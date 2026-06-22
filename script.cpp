#include "script.h"
#include "fs.h"
#include "vga.h"
#include "string.h"
#include "shell.h"
void execute(const char* cmd);

#define MAX_LINE   256
#define MAX_LINES  128

// Split file content into lines
static int split_lines(char* src, char* lines[], int max) {
    int count = 0;
    int i     = 0;
    lines[0]  = src;

    while (src[i] && count < max - 1) {
        if (src[i] == '\n') {
            src[i]       = '\0';
            lines[++count] = &src[i + 1];
        }
        i++;
    }
    return count + 1;
}

// Trim leading spaces
static char* trim(char* str) {
    while (*str == ' ' || *str == '\t') str++;
    return str;
}

// Simple variable store
#define MAX_VARS 16
static char var_names[MAX_VARS][32];
static char var_vals[MAX_VARS][128];
static int  var_count = 0;

static void var_set(const char* name, const char* val) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_names[i], name) == 0) {
            strncpy(var_vals[i], val, 127);
            return;
        }
    }
    if (var_count < MAX_VARS) {
        strncpy(var_names[var_count], name, 31);
        strncpy(var_vals[var_count], val, 127);
        var_count++;
    }
}

static const char* var_get(const char* name) {
    for (int i = 0; i < var_count; i++)
        if (strcmp(var_names[i], name) == 0)
            return var_vals[i];
    return "";
}

// Check if line is a variable assignment (name=value)
static int is_assignment(const char* line) {
    int i = 0;
    while (line[i] && line[i] != '=' && line[i] != ' ') i++;
    return line[i] == '=';
}

// Parse and run a single script line
static void run_line(char* line) {
    line = trim(line);

    // Skip empty lines and comments
    if (!line[0] || line[0] == '#') return;

    // Variable assignment: name=value
    if (is_assignment(line)) {
        char name[32];
        int i = 0;
        while (line[i] && line[i] != '=') { name[i] = line[i]; i++; }
        name[i] = '\0';
        var_set(name, &line[i + 1]);
        return;
    }

    // echo with variable expansion: echo $name
    if (strncmp(line, "echo ", 5) == 0) {
        const char* arg = line + 5;
        if (arg[0] == '$') {
            vga_println(var_get(arg + 1), COLOR_WHITE);
        } else {
            vga_println(arg, COLOR_WHITE);
        }
        return;
    }

    // if name=value then ... end
    if (strncmp(line, "if ", 3) == 0) {
        // handled by script_run block parser
        return;
    }

    // Otherwise treat as shell command
    execute(line);
}

void script_run(const char* filename) {
    static char buf[4096];

    if (fs_read(filename, buf, 4096) < 0) {
        vga_print("Script not found: ", COLOR_LIGHT_RED);
        vga_println(filename, COLOR_LIGHT_RED);
        return;
    }

    char* lines[MAX_LINES];
    int   count = split_lines(buf, lines, MAX_LINES);

    vga_print("Running script: ", COLOR_LIGHT_CYAN);
    vga_println(filename, COLOR_LIGHT_CYAN);

    int i = 0;
    while (i < count) {
        char* line = trim(lines[i]);

        // if condition
        if (strncmp(line, "if ", 3) == 0) {
            // Parse: if var=value
            char* cond     = line + 3;
            char  vname[32];
            char  vval[64];
            int   j = 0;

            while (cond[j] && cond[j] != '=') { vname[j] = cond[j]; j++; }
            vname[j] = '\0';
            strncpy(vval, &cond[j + 1], 63);

            int condition_met = strcmp(var_get(vname), vval) == 0;
            i++;

            // Execute lines until 'end'
            while (i < count) {
                char* inner = trim(lines[i]);
                if (strncmp(inner, "end", 3) == 0) break;
                if (condition_met) run_line(inner);
                i++;
            }
        }

        // repeat N times
        else if (strncmp(line, "repeat ", 7) == 0) {
            int times = 0;
            const char* num = line + 7;
            while (*num >= '0' && *num <= '9') {
                times = times * 10 + (*num - '0');
                num++;
            }
            i++;

            // Collect body lines until 'end'
            int body_start = i;
            int body_end   = i;
            while (body_end < count) {
                char* inner = trim(lines[body_end]);
                if (strncmp(inner, "end", 3) == 0) break;
                body_end++;
            }

            // Execute body N times
            for (int r = 0; r < times; r++)
                for (int k = body_start; k < body_end; k++)
                    run_line(lines[k]);

            i = body_end;
        }

        else {
            run_line(line);
        }

        i++;
    }

    vga_println("Script done.", COLOR_LIGHT_GREEN);
}
