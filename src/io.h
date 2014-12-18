#ifndef IO_H
#define IO_H

#ifndef PASS_MAX
#define PASS_MAX 4096
#endif

int getpass(const char *text, char *out, size_t outlen);
int prompt_yn(const char *message);

#endif
