//
//  main.c
//  cigarette smokers problem
//
//  Created by Vynicius Pontes on 14/11/16.


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t m =	PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t smoker =	PTHREAD_MUTEX_INITIALIZER;

//actors conditionals
pthread_cond_t agent_c =	PTHREAD_COND_INITIALIZER;
pthread_cond_t smoker_tobacco_c =	PTHREAD_COND_INITIALIZER;
pthread_cond_t smoker_match_c =	PTHREAD_COND_INITIALIZER;
pthread_cond_t smoker_paper_c =	PTHREAD_COND_INITIALIZER;

//resource conditionals
pthread_cond_t tobacco =	PTHREAD_COND_INITIALIZER;
pthread_cond_t paper =	PTHREAD_COND_INITIALIZER;
pthread_cond_t match =	PTHREAD_COND_INITIALIZER;

int have_tobacco = 0;
int have_paper = 0;
int have_match = 0;

int agent_job = 1;
int smoker_tobacco_job = 0;
int smoker_match_job = 0;
int smoker_paper_job = 0;

int getRand(int range){
    
    int randNum = rand() % range;
    
    return randNum;
    
}

void * agent(void * arg){
    
    while(1) {
        
        sleep(1);
        
        pthread_mutex_lock(&m);
        
        //The agent stays waiting if agent_job is equal 0
        while(agent_job == 0)
            pthread_cond_wait(&agent_c, &m);
        
        
        printf("-------------------------------\n");
        
        int randNum = getRand(3);
        
        //Paper and Match
        if ( randNum == 0 ) {
           
            agent_job = 0;
            have_match = 1;
            have_paper = 1;
            puts("Put paper and match");
            pthread_cond_signal(&paper);
            pthread_cond_signal(&match);

        }
        //Tobacco and Match
        else if ( randNum == 1 ) {
            agent_job = 0;
            have_match = 1;
            have_tobacco = 1;
            puts("Put tobacco and match");
            pthread_cond_signal(&paper);
            pthread_cond_signal(&match);
        }
        //Tobacco and Paper
        else if ( randNum == 2 ) {
            agent_job = 0;
            have_tobacco = 1;
            have_paper = 1;
            puts("Put paper and tobacco");
            pthread_cond_signal(&paper);
            pthread_cond_signal(&tobacco);
        }
        
        pthread_mutex_unlock(&m);
    }
    return 0;
}

void * pusher_paper(void * arg){
    
    while(1){
        pthread_mutex_lock(&m);
        while(have_paper == 0)
            pthread_cond_wait(&paper, &m);
    
        if(have_match == 1) {
            have_match = 0;
            agent_job = 0;
            smoker_tobacco_job = 1;
            puts("Call the tobacco smoker");
            pthread_cond_signal(&smoker_tobacco_c);
        }
        if(have_tobacco == 1) {
            have_tobacco = 0;
            agent_job = 0;
            smoker_match_job = 1;
            puts("Call the match smoker");
            pthread_cond_signal(&smoker_match_c);
        }
        pthread_mutex_unlock(&m);
    }
    
    return 0 ;
}

void * pusher_match(void * arg){
    
    while(1) {
        pthread_mutex_lock(&m);
        while(have_match == 0)
            pthread_cond_wait(&match, &m);
    
        if(have_paper == 1) {
            have_paper = 0;
            agent_job = 0;
            smoker_tobacco_job = 1;
            puts("Call the tobacco smoker");
            pthread_cond_signal(&smoker_tobacco_c);
        }
        if(have_tobacco == 1) {
            have_tobacco = 0;
            agent_job = 0;
            smoker_paper_job = 1;
            puts("Call the paper smoker");
            pthread_cond_signal(&smoker_paper_c);
        }
        pthread_mutex_unlock(&m);
    }
    
    return 0 ;
}

void * pusher_tobacco(void * arg){
    while(1){
        pthread_mutex_lock(&m);
        while(have_tobacco == 0)
            pthread_cond_wait(&tobacco, &m);
    
        if(have_match == 1) {
            have_match = 0;
            agent_job = 0;
            smoker_paper_job = 1;
            puts("Call the paper smoker");
            pthread_cond_signal(&smoker_paper_c);
        }
        if(have_paper == 1) {
            have_tobacco = 0;
            agent_job = 0;
            smoker_match_job = 1;
            puts("Call the match smoker");
            pthread_cond_signal(&smoker_match_c);
        }
        pthread_mutex_unlock(&m);
    }
    return 0 ;
}

void * smoker_tobacco(void * arg){
    
    while(1){
        
        pthread_mutex_lock(&smoker);
        while(smoker_tobacco_job == 0)
            pthread_cond_wait(&smoker_tobacco_c, &smoker);
        have_paper = 0;
        have_match = 0;
        smoker_tobacco_job = 0;
        agent_job = 1;
        puts("Tobacco Smoker: make cigarette...");
        pthread_mutex_unlock(&smoker);
        
        puts("Tobacco Smoker: Smoking...");
    }
    
    return 0;
}

void * smoker_paper(void * arg){
    
    while(1){
        
        pthread_mutex_lock(&smoker);
        while(smoker_paper_job == 0)
            pthread_cond_wait(&smoker_paper_c, &smoker);
        have_tobacco = 0;
        have_match = 0;
        smoker_paper_job = 0;
        agent_job = 1;
        puts("Paper Smoker: make cigarette...");
        pthread_mutex_unlock(&smoker);
        
        puts("Paper Smoker: Smoking...");
    }
    
    return 0;
}

void * smoker_match(void * arg){
    
    while(1){
        
        pthread_mutex_lock(&smoker);
        while(smoker_match_job == 0)
            pthread_cond_wait(&smoker_match_c, &smoker);
        have_paper = 0;
        have_tobacco = 0;
        smoker_match_job = 0;
        agent_job = 1;
        puts("Match Smoker: make cigarette...");
        pthread_mutex_unlock(&smoker);
        
        puts("Match Smoker: Smoking...");
    }
    
    return 0;
}


int main(int argc, char *argv[])
{
    pthread_t agent_t, smoker_tobacco_t, smoker_paper_t, smoker_match_t, pusher_tobacco_t, pusher_paper_t, pusher_match_t;
    
    //random seed
    time_t t;
    srand((unsigned) time(&t));
    
    if (pthread_create(&agent_t,NULL,agent,NULL) != 0) {
        fprintf (stderr, "Impossible to create thread\n");
        exit (1);
    }
    
    if (pthread_create(&pusher_tobacco_t,NULL,pusher_tobacco,NULL) != 0) {
        fprintf (stderr, "Impossible to create thread\n");
        exit (1);
    }
    
    if (pthread_create(&pusher_paper_t,NULL,pusher_paper,NULL) != 0) {
        fprintf (stderr, "Impossible to create thread\n");
        exit (1);
    }
    
    if (pthread_create(&pusher_match_t,NULL,pusher_match,NULL) != 0) {
        fprintf (stderr, "Impossível criar thread\n");
        exit (1);
    }
    
    if (pthread_create(&smoker_tobacco_t,NULL,smoker_tobacco,NULL) != 0) {
        fprintf (stderr, "Impossible to create thread\n");
        exit (1);
    }
    
    if (pthread_create(&smoker_paper_t,NULL,smoker_paper,NULL) != 0) {
        fprintf (stderr, "Impossible to create thread\n");
        exit (1);
    }
    
    if (pthread_create(&smoker_match_t,NULL,smoker_match,NULL) != 0) {
        fprintf (stderr, "Impossible to create thread\n");
        exit (1);
    }
    
    pthread_join(agent_t, NULL);
    pthread_join(pusher_tobacco_t, NULL);
    pthread_join(pusher_paper_t, NULL);
    pthread_join(pusher_match_t, NULL);
    pthread_join(smoker_tobacco_t, NULL);
    pthread_join(smoker_paper_t, NULL);
    pthread_join(smoker_match_t, NULL);

}
