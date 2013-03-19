#ifndef _LOG_H_
#define _LOG_H_

/*
  need finishing

 */

enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_ERROR,
    LOG_FATAL,
};

void write_log(char log_level, const char* log_str);

#endif
