#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WindowNode {
    Window win;
    char *cls;
    struct WindowNode *next;
} WindowNode;

static WindowNode *windows = nullptr;

void add_window(Window w, const char *cls) {
    WindowNode *n = malloc(sizeof(WindowNode));
    n->win = w;
    n->cls = strdup(cls);
    n->next = windows;
    windows = n;
}

void remove_window(Window w) {
    WindowNode **pp = &windows;
    while (*pp) {
        if ((*pp)->win == w) {
            WindowNode *tmp = *pp;
            *pp = tmp->next;
            free(tmp->cls);
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}

int class_still_open(const char *cls) {
    for (WindowNode *n = windows; n; n = n->next) {
        if (strcmp(n->cls, cls) == 0) return 1;
    }
    return 0;
}

int main(void) {
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    Window root = DefaultRootWindow(dpy);
    XSelectInput(dpy, root, SubstructureNotifyMask);

    for (;;) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        if (ev.type == MapNotify) {
            XMapEvent *e = (XMapEvent *)&ev;
            if (e->event != root) continue;

            XClassHint hint;
            if (XGetClassHint(dpy, e->window, &hint)) {
                if (!class_still_open(hint.res_class)) {
                    printf("New app: %s (%s)\n",
                           hint.res_class, hint.res_name);

                    char cmd[256];
                    snprintf(cmd, sizeof(cmd),
                             "notify-send 'Application running under X11' '%s'",
                             hint.res_class);
                    system(cmd);
                }
                add_window(e->window, hint.res_class);

                if (hint.res_class) XFree(hint.res_class);
                if (hint.res_name) XFree(hint.res_name);
            }
        }

        if (ev.type == DestroyNotify) {
            XDestroyWindowEvent *e = (XDestroyWindowEvent *)&ev;

            WindowNode *n = windows;
            char *cls = nullptr;
            while (n) {
                if (n->win == e->window) {
                    cls = strdup(n->cls);
                    break;
                }
                n = n->next;
            }

            remove_window(e->window);

            if (cls && !class_still_open(cls)) {
                printf("All windows of class %s closed, reset dedup\n", cls);
                free(cls);
            }
        }
    }
}
