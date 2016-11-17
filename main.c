//
//  main.c
//  cigarette smokers problem
//
//  Created by Vynicius Pontes on 14/11/16.
//  auth Vynicius Pontes
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t m =	PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t agentSem =	PTHREAD_COND_INITIALIZER;
pthread_cond_t tobacco =	PTHREAD_COND_INITIALIZER;
pthread_cond_t paper =	PTHREAD_COND_INITIALIZER;
pthread_cond_t match =	PTHREAD_COND_INITIALIZER;

int current_action = 0;

int getRand(int range){
    
    int randNum = rand() % range;
    randNum++;
    return randNum;
    
}

void * agent(void * arg){
    
    while(1) {
        
        sleep(1);
        
        pthread_mutex_lock(&m);
        
        while(current_action != 0)
            pthread_cond_wait(&agentSem, &m);
        
        int randNum = getRand(3);
        
        printf("-------------------------------\n");
        
        if ( randNum == 1 ) {
            // Put tobacco on table
            // Put paper on table
            puts("Agent: Put tobacco on table");
            puts("Agent: Put paper on table");
            current_action = 1;
            pthread_cond_signal(&match);// Wake up smoker with match
        }else if ( randNum == 2 ) {
            puts("Agent: Put tobacco on table");
            puts("Agent: Put match on table");
            current_action = 2;
            pthread_cond_signal(&paper);// Wake up smoker with paper
        }else if ( randNum == 3 )  {
            puts("Agent: Put match on table");
            puts("Agent: Put papper on table");
            current_action = 3;
            pthread_cond_signal(&tobacco);// Wake up smoker with tobacco
        }
        else{
            puts("Wrong rand number");
        }
        pthread_mutex_unlock(&m);
    }
    return 0;
}

void * smoker_match(void * arg){
    
    //smoker match
    while(1){
        pthread_mutex_lock(&m);
        while(current_action != 1)
            pthread_cond_wait(&match, &m);
        puts("Match Smoker: Pick up tobacco");
        puts("Match Smoker: Pick up paper");
        pthread_cond_signal(&agentSem);
        current_action = 0;
        pthread_mutex_unlock(&m);
        puts("Match Smoker: Smoking");
        
    }
    
    return 0;
}

void * smoker_paper(void * arg){
    
    //smoker paper
    while(1){
        pthread_mutex_lock(&m);
        while(current_action != 2)
            pthread_cond_wait(&paper, &m);
        puts("Paper Smoker: Pick up tobacco");
        puts("Paper Smoker: Pick up match");
        pthread_cond_signal(&agentSem);
        current_action = 0;
        pthread_mutex_unlock(&m);
        puts("Paper Smoker: Smoking");
        
    }
    
    return 0;
}

void * smoker_tobacco(void * arg){
    
    //smoker tobacco
    while(1){
        pthread_mutex_lock(&m);
        while(current_action != 3)
            pthread_cond_wait(&tobacco, &m);
        puts("Tobacco Smoker: Pick up match");
        puts("Tobacco Smoker: Pick up paper");
        pthread_cond_signal(&agentSem);
        current_action = 0;
        pthread_mutex_unlock(&m);
        puts("Tobacco Smoker: Smoking");
        
    }
    
    return 0;
}


int main(int argc, char *argv[])
{
    pthread_t agent_t, smoker_tobacco_t, smoker_paper_t, smoker_match_t;
    
    //random seed
    time_t t;
    srand((unsigned) time(&t));
    
    if (pthread_create(&agent_t,NULL,agent,NULL) != 0) {
        fprintf (stderr, "Impossível criar thread\n");
        exit (1);
    }
    
    if (pthread_create(&smoker_tobacco_t,NULL,smoker_tobacco,NULL) != 0) {
        fprintf (stderr, "Impossível criar thread\n");
        exit (1);
    }
    
    if (pthread_create(&smoker_paper_t,NULL,smoker_paper,NULL) != 0) {
        fprintf (stderr, "Impossível criar thread\n");
        exit (1);
    }
    
    if (pthread_create(&smoker_match_t,NULL,smoker_match,NULL) != 0) {
        fprintf (stderr, "Impossível criar thread\n");
        exit (1);
    }
    
    pthread_join(agent_t, NULL);
    pthread_join(smoker_tobacco_t, NULL);
    pthread_join(smoker_paper_t, NULL);
    pthread_join(smoker_match_t, NULL);
    puts("finishing agent");
    puts("finishing smoker");
    
    
    puts("Done");}
