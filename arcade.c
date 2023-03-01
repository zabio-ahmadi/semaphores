#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SIEGE_WAITING_ROOM 8
int coins = 5;
int machineEarn = 0;
const int machineGame = 5;

sem_t mutexArgent, attenteJoueurJoue;
sem_t salleAttente, consoleDisponible, gameover;

void *player(void *arg)
{
    int player_id = *(int *)arg;
    int nb_game_played = 0;

    while (1)
    {

        // simulation of walking on the road
        printf("[player %d] strolls around the city...\n", player_id);
        int walk_time = rand() % 5 + 4; // 4-8 ms
        // sleep((walk_time / 1000));

        printf("[player %d] enters the arcade\n", player_id);
        //-------------------------------------------------------
        // si siègle libre passe le sémaphore, sinon attend
        sem_wait(&salleAttente); //---8
        printf("[player %d] is in waiting room\n", player_id);

        // sémaphore comme mutex pour exclusion mutuelle
        sem_wait(&mutexArgent);
        if (coins > 0)
        {
            coins--; // paiement
            printf("[player %d] pays a coin. Remaining players cash: %d\n", player_id, coins);
            sem_post(&mutexArgent);
        }
        else
        {
            sem_post(&mutexArgent);
            sem_post(&salleAttente);
            printf("[player %d] Leaves the shop because no COINS (nb_games: %d)\n", player_id, nb_game_played);
            return 0;
        }

        sem_wait(&consoleDisponible); // attenteConsoleLibre ou prend place à la console
        sem_post(&salleAttente);      // donc libère salle d'attente

        sem_post(&attenteJoueurJoue);
        printf("[player %d] plays until game is over...\n", player_id);

        nb_game_played++;
        sem_wait(&gameover);
        printf("[player %d] Leaves the shop (nb_games: %d)\n", player_id, nb_game_played);
    }
}

void semaphore_create(sem_t *sem, int nb)
{
    if (sem_init(sem, 0, nb) == -1)
    {
        perror("Erreur lors de l'initialisation du sémaphore\n");
        exit(1);
    }
}

void *console(void *arg)
{
    int cash_machine = 0;
    while (cash_machine != machineGame)
    {
        // La console est prête
        printf("[GAME] waiting player to start...\n");
        sem_post(&consoleDisponible);

        // La console attend qu'un joueur joue
        sem_wait(&attenteJoueurJoue);
        cash_machine++;
        printf("[GAME] start...\n");

        // La console simule une durée de jeu
        // sleep((2 + rand() % 3)/1000);
        sem_post(&gameover);
        printf("[GAME] finished (game over)! Machine cash: %d\n", cash_machine);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc == 1 || atoi(argv[1]) < 1)
    {
        printf("usage : ./arcade {nombre de joueurs}\n");
        printf("Aucun joueur ne peut jouer\n");
        return 0;
    }
    int nb_players = atoi(argv[1]);

    semaphore_create(&salleAttente, 8);
    semaphore_create(&attenteJoueurJoue, 0);
    semaphore_create(&consoleDisponible, 0);
    semaphore_create(&mutexArgent, 1);
    semaphore_create(&gameover, 0);

    // create threads [console, player]
    pthread_t players_threads[nb_players];
    pthread_t console_thread;

    int idJoueur[nb_players];
    for (int i = 0; i < nb_players; i++)
    {
        idJoueur[i] = i + 1;
    }

    // thread pour la console de jeu
    if (pthread_create(&console_thread, NULL, console, NULL))
    {
        printf("Erreur: échec de la création du thread pour la console.\n");
        exit(EXIT_FAILURE);
    }

    // thread pour les M joueurs
    for (int i = 0; i < nb_players; i++)
    {
        int code = pthread_create(&players_threads[i], NULL, player, &idJoueur[i]);
        if (code != 0)
        {
            fprintf(stderr, "pthread_create failed!\n");
            return EXIT_FAILURE;
        }
    }

    // join pour les M joueurs
    for (int i = 0; i < nb_players; i++)
    {
        int code = pthread_join(players_threads[i], NULL);

        if (code != 0)
        {
            fprintf(stderr, "pthread_create failed!\n");
            return EXIT_FAILURE;
        }
    }

    // join pour la console
    if (pthread_join(console_thread, NULL))
    {
        printf("Erreur: échec de l'attente du thread pour la console.\n");
        exit(EXIT_FAILURE);
    }

    sem_destroy(&consoleDisponible);
    sem_destroy(&salleAttente);
    sem_destroy(&mutexArgent);
    sem_destroy(&attenteJoueurJoue);
    sem_destroy(&gameover);

    return 0;
}