#include "search_plugin_tp2b.h"
#include "../curling.c"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TMP_FILE "/tmp/msid_search.json"
#define TMP_DETAIL "/tmp/msid_detail.json"

#define HVSC_API_SEARCH "https://www.hvsc.c64.org/api/v1/sids?q=%s"
#define HVSC_API_DETAIL "https://www.hvsc.c64.org/api/v1/sids/%d"

#define HVSC_MIRROR_BASE "https://hvsc.brona.dk/HVSC/C64Music"

#define BUFSIZE 4096

static gchar *
json_extract_string(const gchar *json,
                    const gchar *key)
{
  gchar pattern[128];
  gchar *p;
  gchar *start;
  gchar *end;

  snprintf(pattern, sizeof(pattern), "\"%s\":\"", key);

  p = strstr(json, pattern);

  if (!p)
    return NULL;

  start = p + strlen(pattern);
  end = strchr(start, '"');

  if (!end)
    return NULL;

  return g_strndup(start, end - start);
}

static int
json_extract_id(const gchar *json)
{
  gchar *p;

  p = strstr(json, "\"id\":");

  if (!p)
    return -1;

  p += 5;

  return atoi(p);
}

static gchar *
read_file_to_string(const gchar *path)
{
  FILE *f;
  long len;
  gchar *buf;

  f = fopen(path, "r");

  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  len = ftell(f);
  fseek(f, 0, SEEK_SET);

  buf = (gchar *) malloc(len + 1);

  fread(buf, 1, len, f);
  buf[len] = '\0';

  fclose(f);

  return buf;
}

static gchar *
build_download_url(const gchar *filePath,
                   const gchar *filename)
{
  return g_strdup_printf("%s%s%s",
                         HVSC_MIRROR_BASE,
                         filePath,
                         filename);
}

GSList *
tp2b_search::search_for_sid(const char *needle,
                            gpointer data)
{
  GSList *entry_list = NULL;

  gchar search_url[1024];

  gchar *json;
  gchar *p;

  DEBUG("HVSC API search for [%s]\n", needle);

  snprintf(search_url,
           sizeof(search_url),
           HVSC_API_SEARCH,
           needle);

  fetch_data_to_file(search_url, TMP_FILE);

  json = read_file_to_string(TMP_FILE);

  if (!json)
    return NULL;

  p = json;

  while ((p = strstr(p, "\"id\":")))
  {
    int id;

    gchar detail_url[1024];

    gchar *detail_json;

    gchar *filePath;
    gchar *filename;

    msid_search_entry *entry;

    id = json_extract_id(p);

    if (id < 0)
    {
      p += 5;
      continue;
    }

    snprintf(detail_url,
             sizeof(detail_url),
             HVSC_API_DETAIL,
             id);

    fetch_data_to_file(detail_url, TMP_DETAIL);

    detail_json = read_file_to_string(TMP_DETAIL);

    if (!detail_json)
    {
      p += 5;
      continue;
    }

    filePath = json_extract_string(detail_json, "filePath");
    filename = json_extract_string(detail_json, "filename");

    if (filePath && filename)
    {
      entry = (msid_search_entry *)
        malloc(sizeof(msid_search_entry));

      entry->uri =
        build_download_url(filePath, filename);

      entry->file_name =
        g_strdup(filename);

      entry->hvsc_path =
        g_strdup_printf("%s%s",
                         filePath,
                         filename);

      entry_list =
        g_slist_prepend(entry_list, entry);
    }

    g_free(filePath);
    g_free(filename);

    free(detail_json);

    p += 5;
  }

  free(json);

  unlink(TMP_FILE);
  unlink(TMP_DETAIL);

  return entry_list;
}

