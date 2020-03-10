/* =========================================================================
 * jhash.h - Jenkins' Hash, maps from string to integer
 *
 * Implementation details can be found at:
 *     http://www.burtleburtle.net/bob/hash/doobs.html
 * ========================================================================= 
 */

#ifndef JHASH_H
#define JHASH_H

/*
 * jenkins_hash - Bob Jenkins' one_at_a_time hash function
 * @str: char buffer to hash (non-NULL)
 * @mod: desired hash modulus (>0)
 *
 * Returns hash(str) % mod. 
 */
unsigned long JenkinsHash(const char *str, const unsigned long mod);

#endif // JHASH_H
