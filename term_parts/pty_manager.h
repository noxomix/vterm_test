#include <pty.h>
#include <unistd.h>
#include <bits/sigaction.h>
#include <signal.h>

int create_pty_fork(Application *app) {
    app->child = forkpty(&app->childfd, NULL, NULL, NULL);
    if(app->child < 0) {
        printf("ERR \n");
        return -1;
    }
    else if(app->child == 0) {
        char *args[] = {"/bin/nano", "main.c", NULL};
        char *dialogArgs[] = {
                "dialog",           // Der Name des Befehls
                "--title",          // Option: Fenstertitel setzen
                "Dialog-Example",  // Fenstertitel
                "--msgbox",         // Dialogtyp: Nachrichtenfeld
                "Hallo von Dialog!",// Nachrichtentext
                "10", "30",         // Höhe und Breite des Dialogs
                NULL
        };
        execvp("/bin/dialog", dialogArgs);
        exit(0);
    } else {
        struct sigaction action = {0};
        action.sa_handler = NULL;
        action.sa_flags = 0;
        sigemptyset(&action.sa_mask);
        sigaction(SIGCHLD, &action, NULL);
        //childState = 1;
    }
}