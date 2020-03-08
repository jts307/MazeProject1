# Webpage module

This module defines the opaque `webpage_t` type, and a suite of functions for creating and manipulating "web pages".  A "web page" is really a struct holding 

 * the URL of this web page, in canonical form
 * the depth at which the crawler found this page
 * the HTML for the page - may be NULL. 

Sometimes you just want to keep track of web pages without HTML - perhaps because you have not yet fetched that HTML - in that case, the webpage object has a null HTML pointer.  

Othertimes, you have fetched the HTML and want to work with it; then, the webpage object has a non-null HTML pointer. 

**For a complete and up-to-date description,** read `webpage.h` for documentation of the interfaces.

## webpage_t

A `webpage_t` object is a struct and exists in one of two states:

 1. initialized, but with no HTML;
 2. initialized, and with HTML fetched from the site.

Initially, on return from `webpage_new()`, the struct is in the first state.  After a successful call to `webpage_fetch`, the struct now has HTML content and is in the second state.

## webpage_new
Creates a new webpage object, given the URL for the web page, the depth at which the crawler encounted this URL, and (optionally) the HTML for this page.

```
webpage_t *webpage_new(char *url, const int depth, char *html);
```

## webpage_delete
Deletes a webpage object and frees its memory.
It takes a `void*` to make it easy to call this from a generic data structure like `bag_delete()`.

```c
void webpage_delete(void *data);
```

## webpage_fetch
Downloads the HTML for the page at the given URL and saves it in the webpage struct.

```c
bool webpage_fetch(webpage_t *page);
```

## webpage_getNextWord
Starts (or continues) a scan of the HTML for the given page, returning the next word in the page.

```c
char *webpage_getNextWord(webpage_t *page, int *pos);
```

## webpage_getNextURL
Starts (or continues) a scan of the HTML for the given page, returning the next URL in the page.

```c
char *webpage_getNextURL(webpage_t *page, int *pos);
```

## NormalizeURL
To *normalize* a URL to canonical form.

```c
bool NormalizeURL(char *url);
```

## IsInternalURL
To determine whether a given URL is "internal" to the CS50 playground.

```c
bool IsInternalURL(char *url);
```

## getter methods

If you must access the contents of the struct, you can request them via one of three getter methods:

```c
int   webpage_getDepth(const webpage_t *page);
char *webpage_getURL(const webpage_t *page);
char *webpage_getHTML(const webpage_t *page);
```
