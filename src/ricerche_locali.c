/***************************************************************************
 *   Copyright (C) 2005 by Antonio Maccioni                                *
 *   antonio.maccioni@gmail.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "rollout.h"


//output: schedula_di_lavoro1, schedula_di_lavoro2
//La funzione restituisce queste due schedule generate rispettivamente da M1_sch ed M2_sch swappando i job sch_swap1 e sch_swap2
void swap(schedula *M1_sch, schedula *M2_sch, elem *M1, elem *M2, schedula *sch_swap1, schedula *sch_swap2, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2){
/*PSEUDOCODICE
1.scorro le schedule copiando gli elementi fino ai job interessati allo swap
2.salvo i 2 job da spostare job_swap1, job_swap2
3.verifica_macchina(schedulazione_di_lavoro1,M1,disponibilità1,setup_vett1,0,job_swap1,0)
  verifica_macchina(schedulazione_di_lavoro2,...)
4.aggiungi_schedula(schedulazione_di_lavoro1,...)
  aggiungi_schedula(schedulazione_di_lavoro2,...)
5.salvo gli ID_violati dei job_swap1 e job_swap2
6.continuo a copiare le schedule fino alla loro fine
*/	
	schedula *temp1;
	schedula *temp2;
	schedula *M1_sch_temp;
	schedula *M2_sch_temp;
	
	job *job_swap1=malloc(sizeof(job));
	job *job_swap2=malloc(sizeof(job));
	job *job_temp=malloc(sizeof(job));
	int *disponibilita1=malloc(sizeof(int));
	int *disponibilita2=malloc(sizeof(int));
	int *setup_vett1=malloc(sizeof(int));
	int *setup_vett2=malloc(sizeof(int));
	int i;

	M1_sch_temp = M1_sch;
	M2_sch_temp = M2_sch;
	temp1=schedula_di_lavoro1;
	temp2=schedula_di_lavoro2;
	
	//copio nella schedula di lavoro1 gli elementi fino a che non incontro quello che dovrei mettere nella schedula_di_lavoro2
	while (M1_sch_temp -> ID_job != sch_swap1 -> ID_job){
		if (temp1->ID_job != -3){
			temp1->next=malloc(sizeof(schedula));
			temp1=temp1->next;
		}
		temp1->ID_job = M1_sch_temp->ID_job;
		temp1->tipo = M1_sch_temp->tipo;
		temp1->inizio = M1_sch_temp->inizio;
		temp1->fine = M1_sch_temp->fine;
		temp1->Lmax = M1_sch_temp->Lmax;
		temp1->Tardy = M1_sch_temp->Tardy;
		temp1->index_camp = M1_sch_temp->index_camp;
		temp1->next=NULL;
		M1_sch_temp = M1_sch_temp->next;
	}

	//copio nella schedula di lavoro2 gli elementi fino a che non incontro quello che dovrei mettere nella schedula_di_lavoro1
	while (M2_sch_temp->ID_job != sch_swap2->ID_job){
		if (temp2->ID_job != -3){
			temp2->next=malloc(sizeof(schedula));
			temp2=temp2->next;
		}
		temp2->ID_job = M2_sch_temp->ID_job;
		temp2->tipo = M2_sch_temp->tipo;
		temp2->inizio = M2_sch_temp->inizio;
		temp2->fine = M2_sch_temp->fine;
		temp2->Lmax = M2_sch_temp->Lmax;
		temp2->Tardy = M2_sch_temp->Tardy;
		temp2->index_camp = M2_sch_temp->index_camp;
		temp2->next=NULL;
		M2_sch_temp = M2_sch_temp->next;
	}

	//fai lo swap
	i=0;
	while(i<num_job)
	{
		if(M1_sch_temp->ID_job == array_job[i].ID){
			job_swap1[0] = array_job[i];
			break;
		}
		i++;
	}
	i=0;
	while(i<num_job)
	{
		if(M2_sch_temp->ID_job == array_job[i].ID){
			job_swap2[0] = array_job[i];
			break;
		}
		i++;
	}

	verifica_macchina(schedula_di_lavoro1,M1,disponibilita1,setup_vett1,0,job_swap2,0);
	verifica_macchina(schedula_di_lavoro2,M2,disponibilita2,setup_vett2,0,job_swap1,0);

	aggiungi_schedula(schedula_di_lavoro1,job_swap2[0],disponibilita1[0],setup_vett1[0]);
	aggiungi_schedula(schedula_di_lavoro2,job_swap1[0],disponibilita2[0],setup_vett2[0]);
	
	//rischedula i job sulle schedule_di_lavoro che sono dopo i job swappati
	M1_sch_temp = M1_sch_temp->next;
	while (M1_sch_temp != NULL){
		i=0;
		while(i<num_job){
			if(M1_sch_temp->ID_job == array_job[i].ID){
				job_temp[0] = array_job[i];
			}
			i++;
		}
		verifica_macchina(schedula_di_lavoro1,M1,disponibilita1,setup_vett1,0,job_temp,0);
		aggiungi_schedula(schedula_di_lavoro1,job_temp[0],disponibilita1[0],setup_vett1[0]);
		M1_sch_temp = M1_sch_temp->next;
		//temp1=temp1->next;
	}

	M2_sch_temp = M2_sch_temp->next;
	//temp2 = temp2->next;
	while(M2_sch_temp != NULL){
		i=0;
		while(i<num_job){
			if(M2_sch_temp->ID_job == array_job[i].ID){
				job_temp[0] = array_job[i];
			}
			i++;
		}
		verifica_macchina(schedula_di_lavoro2,M2,disponibilita2,setup_vett2,0,job_temp,0);
		aggiungi_schedula(schedula_di_lavoro2,job_temp[0],disponibilita2[0],setup_vett1[0]);
		M2_sch_temp = M2_sch_temp->next;
	}
	//libero un pò di memoria
	free(job_swap1);
	free(job_swap2);
	free(job_temp);
	free(disponibilita1);
	free(disponibilita2);
	free(setup_vett1);
	free(setup_vett2);
} 

//output: schedula_di_lavoro1, schedula_di_lavoro2
//Restituisce queste due schedule generate rispettivamente da InCuiInserire col job puntato da sch_insert (di InCuiLevare) e InCuiLevare senza il job puntato da sch_insert
void insert(schedula *InCuiInserire, schedula *InCuiLevare, elem *M1, elem *M2, schedula *sch_insert, schedula *sch_following, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2){
/*PSEUDOCODICE:
1. copio nelle schedule di lavoro i primi elementi se non sono interessati all'insert
2. scorro copiando le schedule fino alle posizioni interessate all'insert
3. faccio l'insert (schedulo in schedula_di_lavoro1 il job_insert)
4. continuo a schedulare la schedula_di_lavoro1 fino alla fine
5. continuo a schedulare la schedula_di_lavoro2 fino alla fine saltando il job_insert
*/

	schedula *temp1;
	schedula *temp2;
	schedula *Sch1_ins_temp;
	schedula *Sch2_ins_temp;
	
	job *job_insert=malloc(sizeof(job));
	job *job_temp=malloc(sizeof(job));
	int *disponibilita=malloc(sizeof(int));
	int *setup_vett=malloc(sizeof(int));
	int i;

	Sch1_ins_temp = InCuiInserire;
	Sch2_ins_temp = InCuiLevare;
	temp1=schedula_di_lavoro1;
	temp2=schedula_di_lavoro2;

	//scorro copiando le schedule fino alle posizioni interessate all'insert
	while (Sch1_ins_temp->ID_job != sch_following->ID_job){
		if (temp1->ID_job != -3){
			temp1->next=malloc(sizeof(schedula));
			temp1=temp1->next;
		}
		temp1->ID_job = Sch1_ins_temp->ID_job;
		temp1->tipo = Sch1_ins_temp->tipo;
		temp1->inizio = Sch1_ins_temp->inizio;
		temp1->fine = Sch1_ins_temp->fine;
		temp1->Lmax = Sch1_ins_temp->Lmax;
		temp1->Tardy = Sch1_ins_temp->Tardy;
		temp1->index_camp = Sch1_ins_temp->index_camp;
		temp1->next=NULL;
		Sch1_ins_temp = Sch1_ins_temp->next;	
	}
	
	while(Sch2_ins_temp -> ID_job != sch_insert -> ID_job){
		if(temp2->ID_job != -3){
			temp2->next=malloc(sizeof(schedula));
			temp2=temp2->next;
		}
		temp2->ID_job = Sch2_ins_temp->ID_job;
		temp2->tipo = Sch2_ins_temp->tipo;
		temp2->inizio = Sch2_ins_temp->inizio;
		temp2->fine = Sch2_ins_temp->fine;
		temp2->Lmax = Sch2_ins_temp->Lmax;
		temp2->Tardy = Sch2_ins_temp->Tardy;
		temp2->index_camp = Sch2_ins_temp->index_camp;
		temp2->next=NULL;
		Sch2_ins_temp = Sch2_ins_temp->next;
	}
	
	//faccio l'insert
	i=0;
	while(i<num_job)
	{
		if(sch_insert->ID_job == array_job[i].ID){
			job_insert[0] = array_job[i];
			break;
		}
		i++;
	}
	verifica_macchina(schedula_di_lavoro1,M1,disponibilita,setup_vett,0,job_insert,0);
	aggiungi_schedula(schedula_di_lavoro1,job_insert[0],disponibilita[0],setup_vett[0]);

	//continuo a schedulare la schedula_di_lavoro1 fino alla fine
	while(Sch1_ins_temp != NULL){
		i=0;
		while(i<num_job){
			if(Sch1_ins_temp -> ID_job == array_job[i].ID){
				job_temp[0]=array_job[i];
				break;
			}
			i++;
		}
		verifica_macchina(schedula_di_lavoro1,M1,disponibilita,setup_vett,0,job_temp,0);
		aggiungi_schedula(schedula_di_lavoro1,job_temp[0],disponibilita[0],setup_vett[0]);
		Sch1_ins_temp=Sch1_ins_temp->next;
	}

	//continuo a schedulare la schedula_di_lavoro2 fino alla fine
	Sch2_ins_temp=Sch2_ins_temp->next;
	while(Sch2_ins_temp != NULL){
		i=0;
		while(i<num_job){
			if(Sch2_ins_temp -> ID_job == array_job[i].ID){
				job_temp[0]=array_job[i];
				break;
			}
			i++;
		}
		verifica_macchina(schedula_di_lavoro2,M2,disponibilita,setup_vett,0,job_temp,0);
		aggiungi_schedula(schedula_di_lavoro2,job_temp[0],disponibilita[0],setup_vett[0]);
		Sch2_ins_temp=Sch2_ins_temp->next;
	}
	//faccio un pò di free
	free(job_insert);
	free(job_temp);
	free(disponibilita);
	free(setup_vett);
}


//esegue le VNS separatamente su M1_sch_buffer, M2_sch_buffer, M3_sch_buffer
int VNS_ricerca_locale(schedula *M1_sch_buffer,schedula *M2_sch_buffer,schedula *M3_sch_buffer, elem *M1, elem *M2, elem *M3)
{
	int ris = 0;
	ris += VNS_per_macchina(M1_sch_buffer,M1);
	if(num_macchine >= 2)
	{	
	ris += VNS_per_macchina(M2_sch_buffer,M2);
	}
	if(num_macchine==3)
	{
		ris += VNS_per_macchina(M3_sch_buffer,M3);
	}
return ris;
}

//output: sch_coda
//Restituisce in sch_coda la schedula generata da M_sch ma col job in_coda spostato in coda
void sposta_in_coda(schedula *M_sch, elem *M, schedula *in_coda, schedula *sch_coda){
	schedula *temp, *M_sch_temp;
	int i;
	int *disponibilita=malloc(sizeof(int));
	int *setup_vett=malloc(sizeof(int));
	job *job_temp=malloc(sizeof(job));
	
	//copio i primi elementi della schedula fino a che non incontro quello da mettere in coda
	temp=sch_coda;
	M_sch_temp=M_sch;
	while(M_sch_temp->ID_job != in_coda->ID_job){
		if (temp->ID_job != -3){
			temp->next=malloc(sizeof(schedula));
			temp=temp->next;
		}
		temp->ID_job = M_sch_temp->ID_job;
		temp->tipo = M_sch_temp->tipo;
		temp->inizio = M_sch_temp->inizio;
		temp->fine = M_sch_temp->fine;
		temp->Lmax = M_sch_temp->Lmax;
		temp->Tardy = M_sch_temp->Tardy;
		temp->index_camp = M_sch_temp->index_camp;
		temp->next=NULL;		
		M_sch_temp=M_sch_temp->next;
	}

	//scavalco l'elemento in coda
	M_sch_temp=M_sch_temp->next;
	//continuo a schedulare fino alla fine
	while (M_sch_temp != NULL){
		i=0;
		while(i<num_job){
			if(M_sch_temp->ID_job == array_job[i].ID){
				job_temp[0] = array_job[i];
			}
			i++;
		}
		verifica_macchina(sch_coda,M,disponibilita,setup_vett,0,job_temp,0);
		aggiungi_schedula(sch_coda,job_temp[0],disponibilita[0],setup_vett[0]);
		M_sch_temp = M_sch_temp->next;	}

	//metto in coda l'elemento in coda
	i=0;
	while(i<num_job){
		if(in_coda->ID_job == array_job[i].ID){
			job_temp[0] = array_job[i];
		}
		i++;
	}
	verifica_macchina(sch_coda, M, disponibilita, setup_vett, 0, job_temp, 0);
	aggiungi_schedula(sch_coda, job_temp[0], disponibilita[0], setup_vett[0]);
	//libera memoria
	free(disponibilita);
	free(setup_vett);
	free(job_temp);
}

//output: schedula_di_lavoro1, schedula_di_lavoro2
//Restituisce gli output con le schedule generate rispettivamente da InCuiInserire con il job puntato da sch_insert (di InCuiLevare) messo in coda e da InCuiLevare senza il job puntato da sch_insert
void insert_coda(schedula *InCuiInserire, schedula *InCuiLevare, elem *M1, elem *M2, schedula *sch_insert, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2){

	schedula *sch1_temp, *sch2_temp, *temp1, *temp2;
	job *job_insert=malloc(sizeof(job));
	job *job_temp=malloc(sizeof(job));
	int i;
	int *disponibilita=malloc(sizeof(int));
	int *setup_vett=malloc(sizeof(int));

	sch1_temp = InCuiInserire;
	sch2_temp = InCuiLevare;
	temp1 = schedula_di_lavoro1;
	temp2 = schedula_di_lavoro2;
	
	//copio in schedula_di_lavoro1 scorrendo InCuiInserire per copiare i valori
	while (sch1_temp != NULL){
		if (temp1->ID_job != -3){
			temp1->next=malloc(sizeof(schedula));
			temp1=temp1->next;
		}
		temp1->ID_job = sch1_temp->ID_job;
		temp1->tipo = sch1_temp->tipo;
		temp1->inizio = sch1_temp->inizio;
		temp1->fine = sch1_temp->fine;
		temp1->Lmax = sch1_temp->Lmax;
		temp1->Tardy = sch1_temp->Tardy;
		temp1->index_camp = sch1_temp->index_camp;
		temp1->next=NULL;
		sch1_temp = sch1_temp->next;
	}

	//copio in schedula_di_lavoro2 scorrendo InCuiLevare fino al job interessato all'insert che dovrà essere rimosso da questa schedula
	while(sch2_temp->ID_job != sch_insert->ID_job){
		if (temp2->ID_job != -3){
			temp2->next=malloc(sizeof(schedula));
			temp2=temp2->next;
		}
		temp2->ID_job = sch2_temp->ID_job;
		temp2->tipo = sch2_temp->tipo;
		temp2->inizio = sch2_temp->inizio;
		temp2->fine = sch2_temp->fine;
		temp2->Lmax = sch2_temp->Lmax;
		temp2->Tardy = sch2_temp->Tardy;
		temp2->index_camp = sch2_temp->index_camp;
		temp2->next=NULL;
		sch2_temp=sch2_temp->next;
	}

	//continuo a schedulare la schedula_di_lavoro2 fino alla fine saltanto il job dell'insert
	sch2_temp=sch2_temp->next;
	while(sch2_temp != NULL){
		i=0;
		while(i<num_job){
			if(sch2_temp -> ID_job == array_job[i].ID){
				job_temp[0]=array_job[i];
				break;
			}
			i++;
		}
		verifica_macchina(schedula_di_lavoro2,M2,disponibilita,setup_vett,0,job_temp,0);
		aggiungi_schedula(schedula_di_lavoro2,job_temp[0],disponibilita[0],setup_vett[0]);
		sch2_temp=sch2_temp->next;
	}

	//faccio l'insert in coda a schedula_di_lavoro1
	i=0;
	while(i<num_job)
	{
		if(sch_insert->ID_job == array_job[i].ID){
			job_insert[0] = array_job[i];
			break;
		}
		i++;
	}
	verifica_macchina(schedula_di_lavoro1,M1,disponibilita,setup_vett,0,job_insert,0);
	aggiungi_schedula(schedula_di_lavoro1,job_insert[0],disponibilita[0],setup_vett[0]);
	//faccio un pò di free
	free(job_insert);
	free(job_temp);
	free(disponibilita);
	free(setup_vett);
}


void inizializza_schedula ( schedula *sch )
{
	sch->ID_job = -3;
	sch->tipo = 0;
	sch->inizio = 0;
	sch->fine = 0;
	sch->Lmax = 0;
	sch->Tardy = 0;
	sch->index_camp = 0;
	sch->next = NULL;	
}

/*ricerca locale tra macchine utilizzando come mosse swap e insert puri.
INSERT: di tutti i job della macchina critica in tutte le posizioni delle altre schedule
SWAP: di tutti i job della macchina critica con tutti quelli delle altre macchine
*/
int ricerca_locale_tra_macchine(schedula *M1_sch, schedula *M2_sch, schedula *M3_sch){

	schedula *temp1, *temp2;
	schedula *schedula_di_lavoro1, *schedula_di_lavoro2;
	quaterna *quaterna_di_lavoro;
	quaterna *quaterna_migliore;
	next_elem *prossimo, *tmp;
	schedula **arraySch;
	elem **arrayM;
	int ris=0;
	printf("\n");
	if(num_macchine == 2){
		prossimo=malloc(sizeof(next_elem));
		quaterna_migliore=malloc(sizeof(quaterna));
		valuta_schedula(M1_sch,M2_sch,NULL,prossimo);
		quaterna_migliore->Feasible=prossimo->feasible;
		quaterna_migliore->Lmax=prossimo->Lmax;
		quaterna_migliore->Cmax=prossimo->Cmax;
		quaterna_migliore->Tardy=prossimo->Tardy;
		quaterna_di_lavoro=malloc(sizeof(quaterna));
		arraySch=calloc(2,sizeof(schedula));;
		arrayM=calloc(2,sizeof(elem));
		ordina_candidati(arraySch,arrayM);
		temp2=arraySch[0];
		while(temp2 != NULL)
		{
			temp1=arraySch[1];
 			while (temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				insert(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,temp1,schedula_di_lavoro1,schedula_di_lavoro2);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}			
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
	
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				swap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;	
		}
	}
	else if(num_macchine==3){
		prossimo=malloc(sizeof(next_elem));
		quaterna_migliore=malloc(sizeof(quaterna));
		valuta_schedula(M1_sch, M2_sch, M3_sch, prossimo);
		quaterna_migliore->Feasible=prossimo->feasible;
		quaterna_migliore->Lmax=prossimo->Lmax;
		quaterna_migliore->Cmax=prossimo->Cmax;
		quaterna_migliore->Tardy=prossimo->Tardy;
		quaterna_di_lavoro=malloc(sizeof(quaterna));
		arraySch=malloc(sizeof(schedula));
		arrayM=malloc(sizeof(elem));
		ordina_candidati(arraySch, arrayM);
		//ciclo di insert tra prima e seconda schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 !=NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				insert(arraySch[1], arraySch[0], arrayM[1], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert4 conveniente    %d   %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax,quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo di insert tra prima e terza schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[2];
			while(temp1 !=NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				insert(arraySch[2], arraySch[0], arrayM[2], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				valuta_schedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2,arraySch[1], schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert5 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&
					(quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2,arraySch[1], schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert6 conveniente    %d    %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax,quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo di swap tra prima e seconda schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				swap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap4 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo di swap tra prima e terza schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[2];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				swap(arraySch[0], arraySch[2], arrayM[0], arrayM[2], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				valuta_schedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap5 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap6 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
	}
	free(quaterna_di_lavoro);
	free(quaterna_migliore);
	elimina_schedula(schedula_di_lavoro1);
	elimina_schedula(schedula_di_lavoro2);

	free(arraySch);
	free(arrayM);
	free(prossimo);

	return ris;
}




/*
ordina le tre schedule in un array a seconda dell'ordine di come andranno esaminate nella ricerca locale.
Quindi in arraySch[0] avrò la schedula critica
*/
void ordina_candidati(schedula **arraySch, elem **arrayM){
	quaterna *quaterna1;
	quaterna *quaterna2;
	quaterna *quaterna3;

	quaterna1=valuta_singola_schedula(M1_sch);
	quaterna2=valuta_singola_schedula(M2_sch);
	if(num_macchine==2){
		if (quaterna1->Lmax >= quaterna2->Lmax){
			arraySch[0]=M1_sch;
			arraySch[1]=M2_sch;
			arrayM[0]=M1;
			arrayM[1]=M2;
		}
		else {
			arraySch[0]=M2_sch;
			arraySch[1]=M1_sch;
			arrayM[0]=M2;
			arrayM[1]=M1;
		}
		free(quaterna1);
		free(quaterna2);
	}
	if(num_macchine == 3){
		quaterna3=valuta_singola_schedula(M3_sch);
		if(quaterna1->Lmax > quaterna2->Lmax && quaterna1->Lmax > quaterna3->Lmax){
			arraySch[0]=M1_sch;
			arraySch[1]=M2_sch;
			arraySch[2]=M3_sch;
			arrayM[0]=M1;
			arrayM[1]=M2;
			arrayM[2]=M3;
		}
		else if(quaterna2->Lmax > quaterna1->Lmax && quaterna2->Lmax > quaterna3->Lmax){
			arraySch[0]=M2_sch;
			arraySch[1]=M1_sch;
			arraySch[2]=M3_sch;
			arrayM[0]=M2;
			arrayM[1]=M1;
			arrayM[2]=M3;
		}
		else{
			arraySch[0]=M3_sch;
			arraySch[1]=M1_sch;
			arraySch[2]=M2_sch;
			arrayM[0]=M3;
			arrayM[1]=M1;
			arrayM[2]=M2;
		}
		free(quaterna1);
		free(quaterna2);
		free(quaterna3);
	}
	
	
}

/*funzione che viene chiamata nel caso di miglioramenti nella ricerca locale tra macchine.
Ricopia le schedule_di_lavoro nelle M_sch.
N.B. Le schedule_di_lavoro vengono messe nell'ordine in cui hanno preso le schedule di arraySch
*/
void sostituisci_schedule(schedula **arraySch, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2, schedula *schedula_di_lavoro3){
	if(num_macchine == 2){
		if(arraySch[0]==M1_sch){
			copia_schelule(schedula_di_lavoro1 , M1_sch);
			copia_schelule(schedula_di_lavoro2 , M2_sch);
		}
		else{
			copia_schelule(schedula_di_lavoro2, M1_sch);
			copia_schelule(schedula_di_lavoro1, M2_sch);
		}
	}
	else if (num_macchine==3){
		if(arraySch[0]==M1_sch && arraySch[1]==M2_sch){
			copia_schelule(schedula_di_lavoro1, M1_sch);
			copia_schelule(schedula_di_lavoro2, M2_sch);
			copia_schelule(schedula_di_lavoro3, M3_sch);
		}
		if(arraySch[0]==M1_sch && arraySch[1]==M3_sch){
			copia_schelule(schedula_di_lavoro1, M1_sch);
			copia_schelule(schedula_di_lavoro2, M3_sch);
			copia_schelule(schedula_di_lavoro3, M2_sch);
		}
		if(arraySch[0]==M2_sch && arraySch[1]==M1_sch){
			copia_schelule(schedula_di_lavoro1, M2_sch);
			copia_schelule(schedula_di_lavoro2, M1_sch);
			copia_schelule(schedula_di_lavoro3, M3_sch);
		}
		if(arraySch[0]==M2_sch && arraySch[1]==M3_sch){
			copia_schelule(schedula_di_lavoro1, M2_sch);
			copia_schelule(schedula_di_lavoro2, M3_sch);
			copia_schelule(schedula_di_lavoro3, M1_sch);
		}
		if(arraySch[0]==M3_sch && arraySch[1]==M1_sch){
			copia_schelule(schedula_di_lavoro1, M3_sch);
			copia_schelule(schedula_di_lavoro2, M1_sch);
			copia_schelule(schedula_di_lavoro3, M2_sch);
		}
		if(arraySch[0]==M3_sch && arraySch[1]==M2_sch){
			copia_schelule(schedula_di_lavoro1, M3_sch);
			copia_schelule(schedula_di_lavoro2, M2_sch);
			copia_schelule(schedula_di_lavoro3, M1_sch);
		}
	}
}

/*ricerca locale tra macchine utilizzando come mosse swap e insert puri e poi facendo sempre la VND su ogni macchine
INSERT: di tutti i job della macchina critica in tutte le posizioni delle altre schedule
SWAP: di tutti i job della macchina critica con tutti quelli delle altre macchine
*/
int ricerca_locale_tra_macchine_VND(schedula *M1_sch, schedula *M2_sch, schedula *M3_sch){
	schedula *temp1, *temp2;
	schedula *schedula_di_lavoro1, *schedula_di_lavoro2;
	quaterna *quaterna_di_lavoro;
	quaterna *quaterna_migliore;
	next_elem *prossimo;
	schedula **arraySch;
	elem **arrayM;
	int ris=0;
	printf("\n");
	if(num_macchine == 2){
		prossimo=malloc(sizeof(next_elem));
		quaterna_migliore=malloc(sizeof(quaterna));
		valuta_schedula(M1_sch,M2_sch,NULL,prossimo);
		quaterna_migliore->Feasible=prossimo->feasible;
		quaterna_migliore->Lmax=prossimo->Lmax;
		quaterna_migliore->Cmax=prossimo->Cmax;
		quaterna_migliore->Tardy=prossimo->Tardy;
		quaterna_di_lavoro=malloc(sizeof(quaterna));
		arraySch=malloc(sizeof(schedula));;
		arrayM=malloc(sizeof(elem));
		ordina_candidati(arraySch,arrayM);
		temp2=arraySch[0];
		while(temp2 != NULL)
		{
			temp1=arraySch[1];
			while (temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				insert(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,temp1,schedula_di_lavoro1,schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[1], arrayM[0], NULL);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}			
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
	
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				swap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;	
		}
	}
	else if(num_macchine==3){
		prossimo=malloc(sizeof(next_elem));
		quaterna_migliore=malloc(sizeof(quaterna));
		valuta_schedula(M1_sch, M2_sch, M3_sch, prossimo);
		quaterna_migliore->Feasible=prossimo->feasible;
		quaterna_migliore->Lmax=prossimo->Lmax;
		quaterna_migliore->Cmax=prossimo->Cmax;
		quaterna_migliore->Tardy=prossimo->Tardy;
		quaterna_di_lavoro=malloc(sizeof(quaterna));
		arraySch=malloc(sizeof(schedula));
		arrayM=malloc(sizeof(elem));
		ordina_candidati(arraySch, arrayM);
		//ciclo di insert tra prima e seconda schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 !=NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				insert(arraySch[1], arraySch[0], arrayM[1], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[1], arrayM[0], NULL);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert4 conveniente   %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo di insert tra prima e terza schedula
		
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[2];
			while(temp1 !=NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				insert(arraySch[2], arraySch[0], arrayM[2], arrayM[0], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[2], arrayM[0], NULL);
				valuta_schedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2,arraySch[1], schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert5 conveniente     %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&
					(quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro2, arraySch[1], schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto un'insert6 conveniente       %d       %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo di swap tra prima e seconda schedula
		
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				swap(arraySch[0], arraySch[1], arrayM[0], arrayM[1], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap4 conveniente    %d    %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo di swap tra prima e terza schedula
		
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[2];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				swap(arraySch[0], arraySch[2], arrayM[0], arrayM[2], temp2, temp1, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[2], NULL);
				valuta_schedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap5 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						ris++;
						printf("Ho fatto uno swap6 conveniente    %d     %d   %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);

						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);

						ris+=ricerca_locale_tra_macchine_VND(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
					}
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
	}
	free(quaterna_di_lavoro);
	free(quaterna_migliore);
	elimina_schedula(schedula_di_lavoro1);
	elimina_schedula(schedula_di_lavoro2);
	free(arraySch);
	free(arrayM);				
	free(prossimo);

	return ris;
}

/*ricerca locale tra macchine utilizzando come mosse swap_coda e insert_coda.
INSERT: di tutti i job della macchina critica in coda alle altre schedule che poi (col nuovo job) subiscono la VND
SWAP: tra tutti i job della macchina critica con tutti quelli delle altre schedule, mettendoli in coda ed eseguendo le due VND su entrambe le schedule
*/
int ricerca_locale_tra_macchine_coda(schedula *M1_sch, schedula *M2_sch, schedula *M3_sch){
	schedula *temp1, *temp2;
	schedula *schedula_di_lavoro1, *schedula_di_lavoro2;
	schedula *sch_coda1, *sch_coda2, *coda1, *coda2;
	quaterna *quaterna_di_lavoro;
	quaterna *quaterna_migliore;
	next_elem *prossimo, *tmp;
	schedula **arraySch;
	elem **arrayM;
	int ris=0;
	printf("\n");
	if (num_macchine==2){
		prossimo=malloc(sizeof(next_elem));
		quaterna_migliore=malloc(sizeof(quaterna));
		valuta_schedula(M1_sch,M2_sch,NULL,prossimo);
		quaterna_migliore->Feasible=prossimo->feasible;
		quaterna_migliore->Lmax=prossimo->Lmax;
		quaterna_migliore->Cmax=prossimo->Cmax;
		quaterna_migliore->Tardy=prossimo->Tardy;
		quaterna_di_lavoro=malloc(sizeof(quaterna));
		arraySch=malloc(sizeof(schedula));;
		arrayM=malloc(sizeof(elem));
		ordina_candidati(arraySch,arrayM);
		temp2=arraySch[0];
		//ciclo di insert
		while (temp2 != NULL){
			schedula_di_lavoro1=malloc(sizeof(schedula));
			schedula_di_lavoro2=malloc(sizeof(schedula));
			inizializza_schedula(schedula_di_lavoro1);
			inizializza_schedula(schedula_di_lavoro2);
			insert_coda(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,schedula_di_lavoro1,schedula_di_lavoro2);
			VNS_per_macchina(schedula_di_lavoro1, arrayM[1]);
			valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);	
			quaterna_di_lavoro->Feasible=prossimo->feasible;
			quaterna_di_lavoro->Lmax=prossimo->Lmax;
			quaterna_di_lavoro->Cmax=prossimo->Cmax;
			quaterna_di_lavoro->Tardy=prossimo->Tardy;
			if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
			{
				if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
				{
					quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
					quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
					quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
					quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
					sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					ris++;
					printf("Ho fatto un'insert1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
					free(arraySch);
					free(arrayM);
					free(quaterna_di_lavoro);
					free(quaterna_migliore);
					free(prossimo);
					ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
					return 1;
					break;
				}
				else if 
				((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
				((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
				((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
				(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
				(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
				{
					quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
					quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
					quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
					quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
					sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, NULL);	
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					ris++;
					printf("Ho fatto un'insert2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
					free(arraySch);
					free(arrayM);
					free(quaterna_di_lavoro);
					free(quaterna_migliore);
					free(prossimo);
					ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
					return 1;
					break;
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
			}
			else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
			}
			temp2=temp2->next;
		}
		//ciclo di swap
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 != NULL){
				
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				sch_coda1=malloc(sizeof(schedula));
				sch_coda2=malloc(sizeof(schedula));
				inizializza_schedula(sch_coda1);
				inizializza_schedula(sch_coda2);
				sposta_in_coda(arraySch[0], arrayM[0], temp2, sch_coda1);
				sposta_in_coda(arraySch[1], arrayM[1], temp1, sch_coda2);
				//coda1, coda2 puntano agli elementi della schedula da swappare, ovvero temp1, temp2 che ora sono stati messi in coda
				coda1=sch_coda1;
				coda2=sch_coda2;
				if(coda1 != NULL){
					while(coda1->next != NULL){
						coda1=coda1->next;
					}
				}
				if(coda2 != NULL){
					while(coda2->next != NULL){
						coda2=coda2->next;
					}
				}
				//sposta i temp in coda alle rispettive schedule
				swap(sch_coda1, sch_coda2, arrayM[0], arrayM[1], coda1, coda2, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, NULL, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
						ris++;
						printf("Ho fatto uno swap1 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);
						ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, NULL);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);					
						ris++;
						printf("Ho fatto uno swap2 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);
						ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					elimina_schedula(sch_coda1);
					elimina_schedula(sch_coda2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
	}
	else if (num_macchine==3){
		prossimo=malloc(sizeof(next_elem));
		quaterna_migliore=malloc(sizeof(quaterna));
		quaterna_di_lavoro=malloc(sizeof(quaterna));
		valuta_schedula(M1_sch, M2_sch, M3_sch, prossimo);
		quaterna_migliore->Feasible=prossimo->feasible;
		quaterna_migliore->Lmax=prossimo->Lmax;
		quaterna_migliore->Cmax=prossimo->Cmax;
		quaterna_migliore->Tardy=prossimo->Tardy;
		arraySch=malloc(sizeof(schedula));;
		arrayM=malloc(sizeof(elem));
		ordina_candidati(arraySch,arrayM);
		temp2=arraySch[0];
		//ciclo di insert tra la prima e la seconda macchina
		while(temp2 != NULL){
			schedula_di_lavoro1=malloc(sizeof(schedula));
			schedula_di_lavoro2=malloc(sizeof(schedula));
			inizializza_schedula(schedula_di_lavoro1);
			inizializza_schedula(schedula_di_lavoro2);
			insert_coda(arraySch[1],arraySch[0],arrayM[1],arrayM[0],temp2,schedula_di_lavoro1,schedula_di_lavoro2);
			VNS_per_macchina(schedula_di_lavoro1, arrayM[1]);
			valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);	
			quaterna_di_lavoro->Feasible=prossimo->feasible;
			quaterna_di_lavoro->Lmax=prossimo->Lmax;
			quaterna_di_lavoro->Cmax=prossimo->Cmax;
			quaterna_di_lavoro->Tardy=prossimo->Tardy;
			if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
			{
				if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
				{
					quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
					quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
					quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
					quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
					sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					ris++;
					printf("Ho fatto un'insert3 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
					free(arraySch);
					free(arrayM);
					free(quaterna_di_lavoro);
					free(quaterna_migliore);
					free(prossimo);
					ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
					return 1;
					break;
				}
				else if 
				((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
				((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
				((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
				(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
				(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
				{
					quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
					quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
					quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
					quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
					sostituisci_schedule(arraySch, schedula_di_lavoro2, schedula_di_lavoro1, arraySch[2]);
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					ris++;
					printf("Ho fatto un'insert4 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
					free(arraySch);
					free(arrayM);
					free(quaterna_di_lavoro);
					free(quaterna_migliore);
					free(prossimo);
					ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
					return 1;
					break;
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
			}
			else{
				elimina_schedula(schedula_di_lavoro1);
				elimina_schedula(schedula_di_lavoro2);
			}
			temp2=temp2->next;
		}
		//ciclo di insert tra la prima e la terza schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			schedula_di_lavoro1=malloc(sizeof(schedula));
			schedula_di_lavoro2=malloc(sizeof(schedula));
			inizializza_schedula(schedula_di_lavoro1);
			inizializza_schedula(schedula_di_lavoro2);
			insert_coda(arraySch[2],arraySch[0],arrayM[2],arrayM[0],temp2,schedula_di_lavoro1,schedula_di_lavoro2);
			VNS_per_macchina(schedula_di_lavoro1, arrayM[2]);
			valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[1], prossimo);	
			quaterna_di_lavoro->Feasible=prossimo->feasible;
			quaterna_di_lavoro->Lmax=prossimo->Lmax;
			quaterna_di_lavoro->Cmax=prossimo->Cmax;
			quaterna_di_lavoro->Tardy=prossimo->Tardy;
			if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
			{
				if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
				{
					quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
					quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
					quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
					quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
					sostituisci_schedule(arraySch, schedula_di_lavoro2, arraySch[1], schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					ris++;
					printf("Ho fatto un'insert5 conveniente");
					free(arraySch);
					free(arrayM);
					free(quaterna_di_lavoro);
					free(quaterna_migliore);
					free(prossimo);
					ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
					return 1;
					break;
				}
				else if 
				((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
				((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
				((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
				(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
				(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
				{
					quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
					quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
					quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
					quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
					sostituisci_schedule(arraySch, schedula_di_lavoro2, arraySch[1], schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					ris++;
					printf("Ho fatto un'insert6 conveniente    %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
					free(arraySch);
					free(arrayM);
					free(quaterna_di_lavoro);
					free(quaterna_migliore);
					free(prossimo);
					ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
					return 1;
					break;
				}
				else
				{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
				}
			}
			else{
				elimina_schedula(schedula_di_lavoro1);
				elimina_schedula(schedula_di_lavoro2);
			}
			temp2=temp2->next;
		}
		//ciclo swap tra la prima e la seconda schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[1];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				sch_coda1=malloc(sizeof(schedula));
				sch_coda2=malloc(sizeof(schedula));
				inizializza_schedula(sch_coda1);
				inizializza_schedula(sch_coda2);
				sposta_in_coda(arraySch[0], arrayM[0], temp2, sch_coda1);
				sposta_in_coda(arraySch[1], arrayM[1], temp1, sch_coda2);
				//coda1, coda2 puntano agli elementi della schedula da swappare, ovvero temp1, temp2 che ora sono stati messi in coda
				coda1=sch_coda1;
				coda2=sch_coda2;
				if(coda1 != NULL){
					while(coda1->next != NULL){
						coda1=coda1->next;
					}
				}
				if(coda2 != NULL){
					while(coda2->next != NULL){
						coda2=coda2->next;
					}
				}
				//sposta i temp in coda alle rispettive schedule
				swap(sch_coda1, sch_coda2, arrayM[0], arrayM[1], coda1, coda2, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[1], NULL);
				valuta_schedula(schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2], prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
						ris++;
						printf("Ho fatto uno swap3 conveniente");
						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);
						ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, schedula_di_lavoro2, arraySch[2]);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
						ris++;
						printf("Ho fatto uno swap4 conveniente    %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);
						ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					elimina_schedula(sch_coda1);
					elimina_schedula(sch_coda2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
		//ciclo swap tra la prima e la terza schedula
		temp2=arraySch[0];
		while(temp2 != NULL){
			temp1=arraySch[2];
			while(temp1 != NULL){
				schedula_di_lavoro1=malloc(sizeof(schedula));
				schedula_di_lavoro2=malloc(sizeof(schedula));
				inizializza_schedula(schedula_di_lavoro1);
				inizializza_schedula(schedula_di_lavoro2);
				sch_coda1=malloc(sizeof(schedula));
				sch_coda2=malloc(sizeof(schedula));
				inizializza_schedula(sch_coda1);
				inizializza_schedula(sch_coda2);
				sposta_in_coda(arraySch[0], arrayM[0], temp2, sch_coda1);
				sposta_in_coda(arraySch[2], arrayM[2], temp1, sch_coda2);
				//coda1, coda2 puntano agli elementi della schedula da swappare, ovvero temp1, temp2 che ora sono stati messi in coda
				coda1=sch_coda1;
				coda2=sch_coda2;
				if(coda1 != NULL){
					while(coda1->next != NULL){
						coda1=coda1->next;
					}
				}
				if(coda2 != NULL){
					while(coda2->next != NULL){
						coda2=coda2->next;
					}
				}
				//sposta i temp in coda alle rispettive schedule
				swap(sch_coda1, sch_coda2, arrayM[0], arrayM[2], coda1, coda2, schedula_di_lavoro1, schedula_di_lavoro2);
				VNS_ricerca_locale(schedula_di_lavoro1, schedula_di_lavoro2, NULL, arrayM[0], arrayM[2], NULL);
				valuta_schedula(schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2, prossimo);	
				quaterna_di_lavoro->Feasible=prossimo->feasible;
				quaterna_di_lavoro->Lmax=prossimo->Lmax;
				quaterna_di_lavoro->Cmax=prossimo->Cmax;
				quaterna_di_lavoro->Tardy=prossimo->Tardy;
				if (quaterna_di_lavoro->Feasible>=quaterna_migliore->Feasible)
				{
					if((quaterna_di_lavoro->Feasible == 1) && (quaterna_migliore->Feasible == 0 ))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
						ris++;
						printf("Ho fatto uno swap5 conveniente    %d     %d     %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);
						ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else if 
					((quaterna_di_lavoro->Lmax<quaterna_migliore->Lmax) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax) &&    (quaterna_di_lavoro->Cmax<quaterna_migliore->Cmax)) ||
					((quaterna_di_lavoro->Lmax==quaterna_migliore->Lmax)&&
					(quaterna_di_lavoro->Cmax==quaterna_migliore->Cmax)&&
					(quaterna_di_lavoro->Tardy<quaterna_migliore->Tardy)))
					{
						quaterna_migliore->Lmax = quaterna_di_lavoro->Lmax;
						quaterna_migliore->Cmax = quaterna_di_lavoro->Cmax;
						quaterna_migliore->Tardy = quaterna_di_lavoro->Tardy;
						quaterna_migliore->Feasible = quaterna_di_lavoro->Feasible;
						sostituisci_schedule(arraySch, schedula_di_lavoro1, arraySch[1], schedula_di_lavoro2);
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
						ris++;
						printf("Ho fatto uno swap6 conveniente     %d     %d    %d",quaterna_di_lavoro->Lmax, quaterna_di_lavoro->Cmax, quaterna_di_lavoro->Tardy);
						free(arraySch);
						free(arrayM);
						free(quaterna_di_lavoro);
						free(quaterna_migliore);
						free(prossimo);
						ris+=ricerca_locale_tra_macchine_coda(M1_sch, M2_sch, M3_sch);
						return 1;
						break;
					}
					else
					{
						elimina_schedula(schedula_di_lavoro1);
						elimina_schedula(schedula_di_lavoro2);
						elimina_schedula(sch_coda1);
						elimina_schedula(sch_coda2);
					}
				}
				else{
					elimina_schedula(schedula_di_lavoro1);
					elimina_schedula(schedula_di_lavoro2);
					elimina_schedula(sch_coda1);
					elimina_schedula(sch_coda2);
				}
				temp1=temp1->next;
			}
			temp2=temp2->next;
		}
	}
	elimina_schedula(schedula_di_lavoro1);
	elimina_schedula(schedula_di_lavoro2);
	free(quaterna_migliore);
	free(quaterna_di_lavoro);
	free(arraySch);
	free(arrayM);	
	free(prossimo);
	return ris;
}
