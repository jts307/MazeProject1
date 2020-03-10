/* 
 * webpage - utility functions for downloading, saving, and loading web pages
 *
 * Ira Ray Jenkins - April 2014
 * 
 * Updated by David Kotz - April 2016, July 2017, April 2019
 * Updated by Temi Prioleau, January 2020
 *
 */

#ifndef __WEBPAGE_H
#define __WEBPAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/***********************************************************************/
/* webpage_t: opaque struct to represent a web page, and its contents.
 */
typedef struct webpage webpage_t;

/* getter methods */
int   webpage_getDepth(const webpage_t *page);
char *webpage_getURL(const webpage_t *page);
char *webpage_getHTML(const webpage_t *page);

/**************** webpage_new ****************/
/* Allocate and initialize a new webpage_t structure.
 * Do NOT fetch the html from url; instead, the
 * caller can fetch it later with webpage_fetch().
 * Parameters:
 *   url   must be a non-null pointer to malloc'd memory.
 *   depth must be non-negative.
 *   html  may be null; if not, must point to malloc'd memory.
 * The pointers url and html are copied, but their strings are NOT copied.
 * They will later be free'd by webpage_delete.
 * 
 * Returns NULL on any error.
 */
webpage_t *webpage_new(char *url, const int depth, char *html);

/**************** webpage_delete ****************/
/* Delete a webpage_t structure created by webpage_new().
 * This function may be called from something like bag_delete().
 * This function calls free() on both the url and the html, if not NULL.
 */
void webpage_delete(void *data);

/***************** webpage_fetch ******************************/
/* retrieve HTML from page->url, store into page->html
 * @page: the webpage struct containing the url to fetch
 *
 * Returns true if the fetch was successful; otherwise, false. 
 * If the fetch succeeded, page->html will contain the content retrieved. 
 * Parameter 'page' should have been allocated by the caller, 
 * but the page->html pointer is expected to be NULL. 
 * If this function is successful, a new, null-terminated character
 * buffer will be allocated as page->html. The caller must later free this
 * memory, typically by calling webpage_delete().
 *
 * Assumptions:
 *     1. page has been allocated by caller
 *     2. page->url contains the url to fetch
 *     3. page->html is NULL at call time
 *
 * Usage example:
 *  webpage_t* page = webpage_new("http://www.example.com", 0, NULL);
 *  if(webpage_fetch(page)) {
 *    char *html = webpage_getHTML(page);
 *    printf("Found html: %s\n", html);
 *  }
 *  webpage_delete(page);
 *
 * Returns:
 *     True: success; caller must later free html via webpage_delete(page).
 *     False: some error fetching page.
 * 
 * Limitations:
 *   * can only handle http (not https or other schemes)
 *   * can only handle URLs of form http://host[:port][/pathname]
 *   * cannot handle redirects (HTTP 301 or 302 response codes)
 */
bool webpage_fetch(webpage_t *page);


/**************** webpage_getNextWord ***********************************/
/* return the next word from html[pos]
 * @page: pointer to the webpage info
 * @pos: current position in html buffer; updated to first pos after the word.
 *
 * Returns pointer to the next word, if any; otherwise, returns NULL.
 * The page should already exist (not NULL), and contain non-NULL html.
 * The *pos argument should be 0 on the initial call; it will be updated to
 *    the position after the word returned.
 * 
 * On successful parse of html, return value will point to a newly allocated
 * buffer; may be NULL on failed return. The caller is responsible for free'ing
 * this memory.
 *
 * Side effect: the page's html will be compressed to remove white space.
 *
 * Usage example: (retrieve all words in a page)
 * int pos = 0;
 * char *result;
 *
 * while ((result = webpage_getNextWord(page, &pos)) != NULL) {
 *     printf("Found word: %s\n", result);
 *     free(result);
 * }
 *
 * Memory contract:
 *     1. inbound, webpage points to an existing struct, with existing html;
 *     2. return value (if not NULL) points to malloc'd space 
 *                   and the caller is responsible for freeing that space.
 */

char *webpage_getNextWord(webpage_t *page, int *pos);

/****************** webpage_getNextURL ***********************************/
/* return the next url from html[pos]
 * @page: pointer to the webpage info
 * @pos: current position in html buffer; updated to first pos after the URL.
 *
 * Returns pointer to the next URL, if any; otherwise, returns NULL.
 * The page should already exist (not NULL), and contain non-NULL html.
 * The *pos argument should be 0 on the initial call; it will be updated to
 *    the position after the URL returned.
 * On successful parse of html, return value will point to a newly allocated
 * buffer; may be NULL on failed return. The caller is responsible for free'ing
 * this memory.
 *
 * Side effect: the page's html will be compressed to remove white space.
 *
 * Usage example: (retrieve all urls in a page)
 * int pos = 0;
 * char *result;
 *
 * while ((result = webpage_getNextURL(page, &pos)) != NULL) {
 *     printf("Found url: %s\n", result);
 *     free(result);
 * }
 *
 * Memory contract:
 *     1. inbound, webpage points to an existing struct, with existing html;
 *     2. return value (if not NULL) points to malloc'd space 
 *                   and the caller is responsible for freeing that space.
 */

char *webpage_getNextURL(webpage_t *page, int *pos);

/***********************************************************************
 * NormalizeURL - attempts to normalize the url
 * @url: absolute url to normalize
 *
 * Returns true on success; 
 * returns false if the url can't be parsed or normalized.
 * returns false if the url refers to a file unlikely to contain html.
 * 
 * Usage example:
 * char* url = calloc(100, sizeof(char));
 * strcpy(url, "HTTP://UsEr:PaSs@www.EXAMPLE.com/path/.././file.html?name=val#top");
 *
 * if (NormalizeURL(url)) {
 *     // url should be: http://UsEr:PaSs@www.example.com/file.html?name=val#top
 * }
 */
bool NormalizeURL(char *url);


/***********************************************************************
 * IsInternalURL - 
 * @url: absolute url to normalize and verify
 *
 * Normalizes URL as a side effect.
 * Returns true if the URL is valid and 'internal' to our domain, 
 * returns false otherwise.
 * 
 * "valid" means that NormalizeURL() returns true;
 * "internal" means that the url begins with INTERNAL_URL_PREFIX.
 */
bool IsInternalURL(char *url);

// All URLs beginning with this prefix are considered "internal"
static const
char INTERNAL_URL_PREFIX[] = "http://old-www.cs.dartmouth.edu";

#endif // __WEBPAGE_H
