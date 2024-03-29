#ifndef __LOG_H__
#define __LOG_H__

#define COLOR_RED           "\033[0;31m"    // Red
#define COLOR_BOLD_RED      "\033[1;31m"	// Bold Red
#define COLOR_GREEN         "\033[0;32m"	// Green
#define COLOR_BOLD_GREEN    "\033[1;32m"	// Bold Green
#define COLOR_YELLOW        "\033[0;33m"	// Yellow
#define COLOR_BOLD_YELLOW   "\033[01;33m"	// Bold Yellow
#define COLOR_BLUE          "\033[0;34m"	// Blue
#define COLOR_BOLD_BLUE     "\033[1;34m"	// Bold Blue
#define COLOR_MAGENTA       "\033[0;35m"	// Magenta
#define COLOR_BOLD_MAGENTA  "\033[1;35m"	// Bold Magenta
#define COLOR_CYAN          "\033[0;36m"	// Cyan
#define COLOR_BOLD_CYAN     "\033[1;36m"	// Bold Cyan
#define COLOR_RESET         "\033[0m"       // Reset

#define log_error(fmt, ...) fprintf(stderr, COLOR_BOLD_RED "[ERROR] " fmt "\n" COLOR_RESET, ##__VA_ARGS__)
#define log_info(fmt, ...) fprintf(stderr, COLOR_BOLD_YELLOW "[INFO] " fmt "\n" COLOR_RESET, ##__VA_ARGS__)
#define log_success(fmt, ...) fprintf(stderr, COLOR_BOLD_GREEN "[SUCCESS] " fmt "\n" COLOR_RESET, ##__VA_ARGS__)
#define log_dbg(level, debug, arg...) \
    do {                              \
        if (debug >= (level))         \
            fprintf(stderr, ##arg);   \
    } while (0)

#endif