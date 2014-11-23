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

//struttura che modella il nodo di una lista di job, la quale è usata come insieme di jobs.
struct ins_din{
	int ID;
	int tipo;
	int proc_time;
	int duedate;
	int deadline;
	int priority;
	int rel_time;	
	struct ins_din *next;
};
typedef struct ins_din insieme_dinamico;

insieme_dinamico *aggiungi_dinamicamente(insieme_dinamico *insieme, job job_insert);
insieme_dinamico *copia_dinamicamente(insieme_dinamico *insieme, insieme_dinamico *insieme_next, job job_insert);


//aggiunge un job in coda ad un insieme dinamico se questo job non è già presente nell'insieme
insieme_dinamico *aggiungi_dinamicamente(insieme_dinamico *insieme, job job_insert){
	insieme_dinamico *temp;
	if(insieme==NULL){
		insieme=malloc(sizeof(insieme_dinamico));
		insieme->ID=job_insert.ID;
		insieme->proc_time=job_insert.proc_time;
		insieme->duedate=job_insert.duedate;
		insieme->deadline=job_insert.deadline;
		insieme->priority=job_insert.priority;
		insieme->rel_time=job_insert.rel_time;
		insieme->next=NULL;
	}
	else{
		temp=insieme;
		while (temp->next != NULL && (temp->ID != job_insert.ID)){
			temp=temp->next;
		}
		if(temp->next == NULL && temp->ID != job_insert.ID){
			temp->next = malloc(sizeof(insieme_dinamico));
			temp=temp->next;
			temp->ID=job_insert.ID;
			temp->proc_time=job_insert.proc_time;
			temp->duedate=job_insert.duedate;
			temp->deadline=job_insert.deadline;
			temp->priority=job_insert.priority;
			temp->rel_time=job_insert.rel_time;
			temp->next=NULL;
		}
		else{
			//se c'era già quel job non fare nulla
		}
	}
	return insieme;
}

//Copia in insieme tutti i job di insieme_next meno job_insert che è quello che viene schedulato dopo il turno di rollout e che quindi non può essere rischedulato
insieme_dinamico *copia_dinamicamente(insieme_dinamico *insieme, insieme_dinamico *insieme_next, job job_insert){
	insieme_dinamico *temp, *temp2;
	temp=insieme;
	temp2=insieme;
	//libero insieme
	while (temp != NULL){
		if (temp->next != NULL){
			temp2=temp->next;
			free(temp);
			temp=temp2;
		}
		else{
			free(temp);
			temp=NULL;
		}
	}
	insieme=temp;
	while (insieme_next != NULL) {
		if (insieme_next->ID != job_insert.ID){
			if(insieme==NULL){
				temp=malloc(sizeof(insieme_dinamico));
				insieme=temp;
				temp->ID=insieme_next->ID;
				temp->proc_time=insieme_next->proc_time;
				temp->duedate=insieme_next->duedate;
				temp->deadline=insieme_next->deadline;
				temp->priority=insieme_next->priority;
				temp->rel_time=insieme_next->rel_time;
				temp->next=NULL;
				temp2=insieme_next;
				insieme_next=insieme_next->next;
				free(temp2);
			}
			else{
				temp->next=malloc(sizeof(insieme_dinamico));
				temp=temp->next;
				temp->ID=insieme_next->ID;
				temp->proc_time=insieme_next->proc_time;
				temp->duedate=insieme_next->duedate;
				temp->deadline=insieme_next->deadline;
				temp->priority=insieme_next->priority;
				temp->rel_time=insieme_next->rel_time;
				temp->next=NULL;
				temp2=insieme_next;
				insieme_next=insieme_next->next;
				free(temp2);
			}			
		}
		else{
			temp2=insieme_next;
			insieme_next=insieme_next->next;
			free(temp2);
		}
	}
	return insieme;
}

//Esegue il rollout col pruning delle euristiche.
//Restituisce la permutazione dei job trovata
job *rollout_heuristic_pruning(int force,char *instance_file)
{

// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo

	next_elem *job_fisso ;
	int i,iter,kk  = 0;
	int pp=0,jj=0;
	int iter_for = 0;
	int index,index1 =0;
	int cont_livelli= num_job-1;
	int Pos_vincitore = 0;
	int Pos_assoluta=0;
	int num_job_relativo = num_job;	
	int j = 0;
	job **permutazioni;
	permutazioni= malloc(num_heur*sizeof(job *));	
	//job *job_da_inserire;
	int primo_passo_M1 = 0;
	int primo_passo_M2 = 0;
	int primo_passo_M3 = 0;
	FILE *f_log;
	char nome_file[25];

	//non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
	job *array_job_attuale;
	job *array_job_attuale_temp;
	schedula *M1_sch_attuale;
	schedula *M1_sch_buffer;
	schedula *M2_sch_buffer;
	schedula *M3_sch_buffer;
	schedula *M1_sch_buffer1;
	schedula *M2_sch_buffer1;
	schedula *M3_sch_buffer1;
	schedula **vettore_schedule_rollout;
	schedula *punt_vett_roll;
	next_elem *lista_prossimi_vincitori; // questo vettore conterra la lista dei vincitori dei singoli turni di rollout 
	M1_sch_attuale = M1_sch;
	schedula *temp1;
	schedula *temp2;
	int fine = 0;
 	permutazione_buffer=malloc(num_job*sizeof(job));
	schedula *M2_sch_attuale; //puntatore all'ultimo elemento della schedula in costruzione su M2
	if(num_macchine >= 2)
	{
		M2_sch_attuale = M2_sch;
	}
	schedula *M3_sch_attuale;
	if(num_macchine == 3)
	{	
		M3_sch_attuale = M3_sch;
	}	
	array_job_attuale = malloc(num_job*sizeof(job));
	
	for(i = 0;i<num_job;i++)
	{
		array_job_attuale[i] = array_job[i];
	}
	job_fisso = (next_elem *)malloc(sizeof(next_elem));
	job * perm_di_passaggio;
	//WHILE-STADI-ROLLOUT
	while(cont_livelli>0)
	{
		next_elem lista_prossimi_vincitori[500];
		index=0;
		index1=0;
		/*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/		
		for(iter_for=0;iter_for<num_job;iter_for++)
		{	
			if (array_job_attuale[iter_for].ID==-1)//se �non selezionabile
			{
			}
			else //se �selezionabile
			{					
				array_job_attuale[iter_for].ID =-1;
				array_job_attuale_temp=malloc((cont_livelli)*sizeof(job));
				iter=0;
				//mette in array_job_attuale_temp i primi cont_livelli job selezionabili, cioè i job che devono essere ancora schedulati
				for(kk=0;kk<cont_livelli;kk++)
				{
					while(array_job_attuale[iter].ID==-1)
					{
						iter++;
					}
					array_job_attuale_temp[kk]=array_job_attuale[iter];
					iter++;
				}
				iter=0;
				next_elem *prossimo = NULL;
				next_elem *prossimo1;
				for (i = 0;i<num_heur;i++)//num_heur dovr�essere cambiato con il numero di heuristiche effettivamente usato
				{
					if (funzioni[i].perc_utilizzo !=-1)
					{
						azzera_schedule();
						perm_di_passaggio=malloc(num_job*sizeof(job));
						inizializza_perutazione_migliore(perm_di_passaggio); 
						for(pp=0;pp<num_job-cont_livelli-1;pp++)
						{
							perm_di_passaggio[pp]=best_perm[pp];
						}
						pp++;
						perm_di_passaggio[(num_job-cont_livelli-1)]=array_job[iter_for];
						next_elem *prossimo2;
				 		prossimo2= malloc(sizeof(next_elem));
						prossimo2->next=NULL; 
						costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
						//prossimo2 contiene le informazioni su questa schedulazione con perm_di_passaggio
						free(prossimo2);
						azzera_schedule();
						permutazioni[i]=NULL;				
						permutazioni[i] = funzioni[i].funz(array_job_attuale_temp,cont_livelli);
						//si scrive nelle righe di permutazioni la permutazione trovata dalla i-esima euristica
						//se prossimo è NULL si inizializza come testa della lista altrimenti si aggiunge in cosa un nuovo elemento temp
						//si aggiunge un next_elem col primo job della permutazione trovata dall'euristica e relativi dati della schedula parziale
						if(prossimo == NULL)
						{
							prossimo = malloc(sizeof(next_elem));
							prossimo->ID_job = permutazioni[i][0].ID;
							prossimo->ID_heur= funzioni[i].ID_heur;
							prossimo->next = NULL;			
						}
						else
						{
							next_elem *temp;
							next_elem *temp_prox;
							temp = prossimo;
							while(temp->next!=NULL)
							{
								temp = temp->next;
							}
							temp_prox=malloc(sizeof(next_elem));
							temp->next = temp_prox;
							temp = temp->next;
							temp->ID_job = permutazioni[i][0].ID;
							temp->ID_heur= funzioni[i].ID_heur;
							temp->next = NULL;
		
						}
						//completo la fine perm_di_passaggio con la permutazione appena trovata dall'euristica
						for(jj=0;pp<num_job;pp++,jj++)
						{
							perm_di_passaggio[pp]=permutazioni[i][jj];
						}					
						azzera_schedule();
						//costruisce e valuta la schedula ottenuta dalla perm_di_passaggio salvando i risultati ottenuti in prossimo
						costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job);
						printf(" %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli);
						salva_se_meglio(perm_di_passaggio,best_perm,cont_livelli,iter_for);
						free(perm_di_passaggio);
						//devo riportare la macchina nelle condizioni orginarie
					}
					else
					{
						//se la euristica è stata esclusa in precedenza...
					}			
				}//FINE for i<num_heur
				//se la schedula non �feasible deve essere penalizzata rispetto alle altre.
				//devo ridurre il numero di job che rimangono da schedulare
				//devo trovare il job con la Lateness + alta 
				//in condizioni di parit�quello con la Cmax +bassa
				// infine con il numero + basso di Tardy job

				//imposto i valori di scheduling (var globali) con i valori di prossimo (della testa della lista), cioè gli inizializzo prima dello scorrimento della lista
				array_job_attuale[iter_for].ID =iter_for;
				free(array_job_attuale_temp);
				int ID_job = prossimo->ID_job;
				int macchina = prossimo->macchina;
				int fine = prossimo->fine;
				int inizio = prossimo->inizio;
				int tipo = prossimo->tipo;
				int index_camp = prossimo->index_camp;
				int LMAX = prossimo->Lmax_pers;
				int TARDY = prossimo->Tardy_pers;
				int L_max = prossimo->Lmax;
				int C_max = prossimo->Cmax;
				int tardy = prossimo->Tardy;
				int ID_heur = prossimo->ID_heur;
				int rel_time = prossimo->rel_time;
				int proc_time = prossimo->proc_time;
				int duedate = prossimo->duedate;
				int deadline = prossimo->deadline;
				int priority = prossimo->priority;
				Feasible = prossimo->feasible;		
				next_elem *temp = prossimo ;
				int c=1; //variabile che conta per sapere i vincitori locali
				int vinc_loc=ID_heur; //variabile che tiene traccia del vincitore locale;
				if (c==num_heur_utilizz){
					funzioni[vinc_loc].perc_utilizzo++;
					printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
				}
				//WHILE-VINCITORI-EURISTICHE
				while(temp->next!=NULL)	
				{
					if (Feasible <= temp->next->feasible)		
						{
						if
						(
							(Feasible ==0)
							&&
							(temp->next->feasible==1)
						)
						{
							ID_job = temp->next->ID_job; 
							macchina = temp->next->macchina;
							tipo = temp->next->tipo;
							fine = temp->next->fine;
							inizio = temp->next->inizio;
							index_camp = temp->next->index_camp;
							LMAX = temp->next->Lmax_pers;
							TARDY = temp->next->Tardy_pers;
							L_max = temp->next->Lmax;
							C_max = temp->next->Cmax;
							tardy = temp->next->Tardy;				
							deadline = temp->next->deadline;
							duedate = temp->next->duedate;
							proc_time = temp->next->proc_time;
							rel_time = temp->next->rel_time;
							priority = temp->next->priority;
							ID_heur = temp->next->ID_heur;
							Feasible = temp->next->feasible;
							c++;
							vinc_loc=ID_heur;
							if (c==num_heur_utilizz){
								funzioni[vinc_loc].perc_utilizzo++;
								printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
								c=0;
							}
						}
						else if
						(	
							(L_max > temp->next->Lmax)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max > temp->next->Cmax)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy > temp->next->Tardy)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&
								(LMAX > temp->next->Lmax_pers)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&
								(LMAX > temp->next->Lmax_pers)
								&&
								(
								(temp->next->duedate !=0)
								&&
								(duedate > temp->next->duedate)
								)					
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(
								(temp->next->deadline !=0)
								&&
								(deadline > temp->next->deadline)
								)						
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(deadline == temp->next->deadline)
								&&
								(proc_time > temp->next->proc_time)		
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(deadline == temp->next->deadline)
								&&
								(proc_time == temp->next->proc_time)
								&&
								(rel_time > temp->next->proc_time)		
							)	
						)		
						{
							ID_job = temp->next->ID_job; 
							macchina = temp->next->macchina;
							tipo = temp->next->tipo;
							fine = temp->next->fine;
							inizio = temp->next->inizio;
							index_camp = temp->next->index_camp;
							LMAX = temp->next->Lmax_pers;
							TARDY = temp->next->Tardy_pers;
							L_max = temp->next->Lmax;
							C_max = temp->next->Cmax;
							tardy = temp->next->Tardy;				
							deadline = temp->next->deadline;
							duedate = temp->next->duedate;
							proc_time = temp->next->proc_time;
							rel_time = temp->next->rel_time;
							priority = temp->next->priority;
							ID_heur = temp->next->ID_heur;
							Feasible = temp->next->feasible;
							c++;
							vinc_loc=ID_heur;
							if (c==num_heur_utilizz){
								funzioni[vinc_loc].perc_utilizzo++;
								printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
								c=0;
							}
						}
						else{
							if (c!=num_heur_utilizz)
								c++;
							else {
								c=1;
								funzioni[vinc_loc].perc_utilizzo++;
								printf("La funzione %i rivince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
								//se tutte le euristiche del turno non portano benefici si considera vincitore l'ultimo che è stato
							}
						}				
					}
					else {
						if (c!=num_heur_utilizz)
							c++;
						else {
							c=1;
							funzioni[vinc_loc].perc_utilizzo++;
							printf("La funzione %i rivince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
							//se tutte le euristiche del turno sono non feasible si considera vincitore l'ultimo che è stato
						}
					}
					temp=temp->next;		
				}//FINE WHILE-VINCITORI-EURISTICHE:scorre temp (vecchio prossimo) salvando di volta in volta valori migliori tra tutte le schedule contenute in prossimo, che erano quelle trovate da tutte le euristiche tenendo questo job come fisso
				//mio codice per vincitori locali
				if (c==num_heur_utilizz && num_heur_utilizz !=1){
					funzioni[vinc_loc].perc_utilizzo++;
					printf("La funzione %i vince e la sua perc è: %i \n",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
				}
				//ora sappiamo qual �L'ID del Job da eliminare dalla lista dei job da schedulare
				// e su quale macchina �stato schedulato	

 				printf("\n(%i) %i %i %i %i",cont_livelli, ID_heur, L_max, C_max,tardy);
				printf("\n");				
				// salvataggio nella lista delle info sul job prescelto in questo step		
				lista_prossimi_vincitori[index].ID_job = ID_job; 
				lista_prossimi_vincitori[index].macchina = macchina;
				lista_prossimi_vincitori[index].tipo =  tipo;
				lista_prossimi_vincitori[index].fine = fine;
				lista_prossimi_vincitori[index].inizio = inizio;
				lista_prossimi_vincitori[index].index_camp = index_camp;
				lista_prossimi_vincitori[index].Lmax_pers = LMAX;
				lista_prossimi_vincitori[index].Tardy_pers =TARDY;
				lista_prossimi_vincitori[index].Lmax = L_max;
				lista_prossimi_vincitori[index].Cmax = C_max;
				lista_prossimi_vincitori[index].Tardy = tardy;	
				lista_prossimi_vincitori[index].deadline = deadline;
				lista_prossimi_vincitori[index].duedate = duedate;
				lista_prossimi_vincitori[index].proc_time = proc_time;
				lista_prossimi_vincitori[index].rel_time = rel_time;
				lista_prossimi_vincitori[index].priority = priority;
				lista_prossimi_vincitori[index].ID_heur = ID_heur;
				lista_prossimi_vincitori[index].feasible = Feasible;
				index++;
				// fine salvataggio ___________________________________________________________	
				
				//libero prossimo e permutazioni per il prossimo ciclo
				next_elem *temp_pr;
				while (prossimo->next != NULL){
					temp_pr = prossimo->next;
					free(prossimo);
					prossimo = temp_pr;
				}

				for(i=0;i<num_heur;i++)
				{
					if (funzioni[i].perc_utilizzo != -1)
						free(permutazioni[i]);
				}	
			}
		} //FINE for con iter_for (quello che fa partire il rollout da tutti i job)
 		printf(" %i____________________\n",cont_livelli);				
//____________________________________________________________________________________________________________________		

		Pos_vincitore=seleziona_prossimo_job(lista_prossimi_vincitori,cont_livelli);
		//Pos_vincitore indica la posizione relativa del job da schedulare
		//scelto fra tutti quelli vincitori in questo stadio
		Pos_assoluta=trova_posizione_assoluta(array_job_attuale,Pos_vincitore);

		// devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
		//poi inserisco in best_perm il job migliore fra tutti i vincitori dello stadio di rollout
		if
		(
			(lista_prossimi_vincitori[Pos_vincitore].feasible>Feasible_best)
		||
			(
				(lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best)
				&&
				(lista_prossimi_vincitori[Pos_vincitore].Lmax<Lmax_best)
			)
		||
			(
				(lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best)
				&&
				(lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best)
				&&
				(lista_prossimi_vincitori[Pos_vincitore].Cmax<Cmax_best)
			)
		||
			(
				(lista_prossimi_vincitori[Pos_vincitore].feasible==Feasible_best)
				&&
				(lista_prossimi_vincitori[Pos_vincitore].Lmax==Lmax_best)
				&&
				(lista_prossimi_vincitori[Pos_vincitore].Cmax==Cmax_best)
				&&
				(lista_prossimi_vincitori[Pos_vincitore].Tardy<Tardy_best)
			)
		)
		{
			best_perm[num_job-cont_livelli-1].ID=array_job_attuale[Pos_assoluta].ID;
			best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
			best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
			best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
			best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
			//il job schedulato viene segnato come non selezionabile (non rischedulabile)
			array_job_attuale[Pos_assoluta].ID=-1;		
		}		
		else
		{
			best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;			
			best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
			best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
			best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
			best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
			array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;
		} 
		//ora viene esclusa una euristica dall'insieme delle euristiche a seconda della politica di pruning scelta.
		escludi_euristica(funzioni,politica_pruning,cont_livelli);

		cont_livelli--;      
	} //FINE WHILE-STADI-ROLLOUT
	
	i=0;
	while(array_job_attuale[i].ID==-1)
	{
		i++;
	}
	best_perm[num_job-1].ID=array_job_attuale[i].ID;
	best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
	best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
	best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
	best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
	best_perm[num_job-1].priority=array_job_attuale[i].priority;
	best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
	array_job_attuale[i].ID=-1;
	//libero memoria che non mi serve più
	free(array_job_attuale);
	azzera_schedule();
	next_elem *prossimo1;
 	prossimo1= malloc(sizeof(next_elem));
	prossimo1->next=NULL; 
	//costruisci e valuta la permutazione finale, confronto con quella buffer e restituisco la permutazione trovata dall'intero rollout
	costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job);
	free(prossimo1);	
	if
	(
		(Feasible<Feasible_best)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax>Lmax_best)
		)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax==Lmax_best)
			&&
			(Cmax>Cmax_best)
		)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax==Lmax_best)
			&&
			(Cmax==Cmax_best)
			&&
			(Tardy>Tardy_best)
		)
	)
	{
		free(permutazioni);
		return permutazione_buffer;
	}		
	else
	{
		free(permutazioni);
		free(permutazione_buffer);
		return	best_perm;
	}
}



/*calcola le statistiche relative all'utilizzo delle funzioni
Restituisce un vettore con le percentuali d'utilizzo per ogni funzione euristica
*/
float *calcola_statistiche()
{
	int i, somma=0;
	float k;
	float *stat;
	stat=malloc(num_heur*sizeof(float));
	for (i=0;i<num_heur;i++)
	{
		if (funzioni[i].perc_utilizzo!=-1)
			somma+=funzioni[i].perc_utilizzo;
	}
    	
	for (i = 0;i<num_heur;i++)
	{
		if(funzioni[i].perc_utilizzo!=-1)
		{	
			stat[i]=((float)funzioni[i].perc_utilizzo/somma)*100.0;
		}
		else
		{
			stat[i]=0.0;
		}
	}	
	return stat;
}

/*sceglie quale politica di esclusione delle euristiche applicare*/
void escludi_euristica(strutt_funz *funzioni, int politica,int cont_livelli)
{
	switch (politica)
	{
	case 0:
	{
		//no exclusion
		break;
	}
	case 1:
	{
		politica_never_win(funzioni);
		break;
	}
	case 2:
	{
		politica_less3perc_reset(funzioni);
		break;

	}
	case 3:
	{
		politica_less5perc_reset(funzioni);
		break;

	}
	case 4:
	{
		politica_less7perc_reset(funzioni);
		break;

	}
	case 5:
	{
		politica_less3perc(funzioni);
		break;

	}
	case 6:
	{
		politica_less5perc(funzioni);
		break;

	}
	case 7:
	{
		politica_less7perc(funzioni);
		break;

	}
	case 8:
	{
		politica_linear_spread(funzioni, cont_livelli);
		break;
	}
	case 9:
	{
		politica_linear_spread_reset(funzioni, cont_livelli);
		break;
	}
	case 10:
	{
		politica_never_win_one_at_a_time(funzioni);
		break;
	}
	}
}

/*esclude dalle euristiche da applicare quelle che dopo uno stadio di rollout non hanno mai vinto*/
void politica_never_win(strutt_funz *funzioni)
{
	int i;
	float *percentuali;
	int flag=0;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]==0.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);
			flag=1;
		}
	}
	//reset
	if (flag==1)
		for(i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo!=-1)
				funzioni[i].perc_utilizzo=0;
		}
	printf("\n");
}

/*esclude quelle euristiche che dopo uno stadio di rollout hanno vinto meno del 3% delle volte*/
void politica_less3perc(strutt_funz *funzioni)
{
	int i, flag=0;
	float *percentuali;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]<3.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);	
			flag=1;
		}
	}
	printf("\n");
}

/*esclude quelle euristiche che dopo uno stadio di rollout hanno vinto meno del 5% delle volte*/
void politica_less5perc(strutt_funz *funzioni)
{
	int i, flag=0;
	float *percentuali;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]<5.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);	
			flag=1;
		}
	}
	printf("\n");
}

/*esclude quelle euristiche che dopo uno stadio di rollout hanno vinto meno del 7% delle volte*/
void politica_less7perc(strutt_funz *funzioni)
{
	int i, flag=0;
	float *percentuali;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]<7.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);	
			flag=1;
		}
	}
	printf("\n");
}

/*esclude quelle euristiche che hanno vinto meno del 3% delle volte in quello stadio di rollout*/
void politica_less3perc_reset(strutt_funz *funzioni)
{
	int i, flag=0;
	float *percentuali;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]<3.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);	
			flag=1;
		}
	}
	//reset
	if (flag==1 && num_heur_utilizz > 1)
		for(i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo!=-1)
				funzioni[i].perc_utilizzo=0;
		}
	printf("\n");
}

/*esclude quelle euristiche che hanno vinto meno del 5% delle volte in quello stadio di rollout*/
void politica_less5perc_reset(strutt_funz *funzioni)
{
	int i, flag=0;
	float *percentuali;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]<5.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);	
			flag=1;
		}
	}
	//reset
	if (flag==1 && num_heur_utilizz > 1)
		for(i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo!=-1)
				funzioni[i].perc_utilizzo=0;
		}
	printf("\n");
}

/*esclude quelle euristiche che hanno vinto meno del 7% delle volte in quello stadio di rollout*/
void politica_less7perc_reset(strutt_funz *funzioni)
{
	int i, flag=0;
	float *percentuali;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]<7.0) && (funzioni[i].perc_utilizzo != -1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);	
			flag=1;
		}
	}
	//reset
	if (flag==1 && num_heur_utilizz > 1)
		for(i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo!=-1)
				funzioni[i].perc_utilizzo=0;
		}
	printf("\n");
}

/*esclude l'euristica peggiore dopo ogni (num_job/num_heur) volte
* in questo modo negli ultimi stadi resta solo l'euristica migliore
*/
void politica_linear_spread(strutt_funz *funzioni, int cont_livelli){
	int i, j, tmp, rank[num_heur], somma=0;
		
	if ((num_heur>num_job) || ((num_job-cont_livelli)%(num_job/num_heur)==0 && (num_heur_utilizz>1))) {
		//calcola il ranking
		for (i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo != -1)
				somma+=funzioni[i].perc_utilizzo;
		}
		for(i=0;i<num_heur;i++)
			rank[i]=i;
		for(i=1;i<num_heur;i++)
		{
        		for(j=0;j<num_heur;j++) 
			{
                		if (funzioni[rank[i]].perc_utilizzo>funzioni[rank[j]].perc_utilizzo) {
					tmp=rank[j];
					rank[j]=rank[i];
					rank[i]=tmp;
				}
        		}
		}
		//trova indice j del rank da eliminare
		j=-1;
		i=num_heur-1;
		while(i>0 && j==-1){
			if(funzioni[rank[i]].perc_utilizzo != -1)
				j=i;
			i--;
		}
		//elimina la funzione che tra quelle attualmente utilizzate ha una percentuale di utilizzo minore
		funzioni[rank[j]].perc_utilizzo=-1;
		printf("\n Escludo l'euristica numero %i\n",rank[j]);
		num_heur_utilizz--;
	}
}

/*esclude l'euristica peggiore dopo ogni (num_job/num_heur) volte
* in questo modo negli ultimi stadi resta solo l'euristica migliore 
* faccio il reset delle percentuali d'utilizzo dopo ogni stadio di rollout
* la funzione assume un carattere simile a quello del racing (esclusione a confronti)
*/
void politica_linear_spread_reset(strutt_funz *funzioni, int cont_livelli){
	int i, j, tmp, rank[num_heur], somma=0;
		
	if ((num_heur>num_job) || ((num_job-cont_livelli)%(num_job/num_heur)==0 && (num_heur_utilizz>1))) {
		//calcola il ranking
		for (i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo != -1)
				somma+=funzioni[i].perc_utilizzo;
		}
		for(i=0;i<num_heur;i++)
			rank[i]=i;
		for(i=1;i<num_heur;i++)
		{
        		for(j=0;j<num_heur;j++) 
			{
                		if (funzioni[rank[i]].perc_utilizzo>funzioni[rank[j]].perc_utilizzo) {
					tmp=rank[j];
					rank[j]=rank[i];
					rank[i]=tmp;
				}
        		}
		}
		//trova indice j del rank da eliminare
		j=-1;
		i=num_heur-1;
		while(i>0 && j==-1){
			if(funzioni[rank[i]].perc_utilizzo != -1)
				j=i;
			i--;
		}
		//elimina la funzione che tra quelle attualmente utilizzate ha una percentuale di utilizzo minore
		funzioni[rank[j]].perc_utilizzo=-1;
		printf("\n Escludo l'euristica numero %i\n",rank[j]);
		num_heur_utilizz--;
		//reset
		for(i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo!=-1)
				funzioni[i].perc_utilizzo=0;
		}
	}
}

/*esclude una tra le euristiche che dopo uno stadio di rollout non hanno mai vinto*/
void politica_never_win_one_at_a_time(strutt_funz *funzioni)
{
	int i;
	float *percentuali;
	int flag=0;

	percentuali=malloc(num_heur*sizeof(float));	
	percentuali=calcola_statistiche();
	printf("\n");
	//scelgo le euristiche da eliminare
	for (i=0;i<num_heur;i++)
	{
		if ((percentuali[i]==0.0) && (funzioni[i].perc_utilizzo != -1) && (flag != 1))
		{
			funzioni[i].perc_utilizzo=-1;
			num_heur_utilizz--;
			printf("\n Escludo l'euristica numero %i\n",i);
			flag=1;
		}
	}
	//reset
	if ((flag==1) && (num_heur_utilizz > 1))
		for(i=0;i<num_heur;i++){
			if (funzioni[i].perc_utilizzo!=-1)
				funzioni[i].perc_utilizzo=0;
		}
	printf("\n");
}



/*
Esegue il rollout con la scelta dinamica dei job ad ogni stadio di rollout
Restituisce una permutazione dei job
*/
job *rollout_dynamic_job_choosing ( int force,char *instance_file )
{
// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo
	int i,iter,kk  = 0;
	int pp=0,jj=0;
	int iter_for = 0;
	int cont_livelli= num_job-1;
	int Pos_vincitore = 0;
	int Pos_assoluta=0;
	int num_job_relativo = num_job;	
	int j = 0;
	job **permutazioni;
	permutazioni= malloc(num_heur*sizeof(job *));	
	job *job_da_inserire;
	int primo_passo_M1 = 0;
	int primo_passo_M2 = 0;
	int primo_passo_M3 = 0;
	FILE *f_log;
	char nome_file[25];
	insieme_dinamico *insieme, *insieme_next, *insieme_temp;
	//non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
	job *array_job_attuale;
	job *array_job_attuale_temp;
	schedula *M1_sch_attuale;
	schedula *M1_sch_buffer;
	schedula *M2_sch_buffer;
	schedula *M3_sch_buffer;
	schedula *M1_sch_buffer1;
	schedula *M2_sch_buffer1;
	schedula *M3_sch_buffer1;
	schedula **vettore_schedule_rollout;
	schedula *punt_vett_roll;
	next_elem *lista_vincitori=NULL; //contiene i vincitori dei singoli turni dello stadio di rollout
	M1_sch_attuale = M1_sch;
	schedula *temp1;
	schedula *temp2;
	int fine = 0;
 	permutazione_buffer=malloc(num_job*sizeof(job));
	schedula *M2_sch_attuale; //puntatore all'ultimo elemento della schedula in costruzione su M2
	if(num_macchine >= 2)
	{
		M2_sch_attuale = M2_sch;
	}
	schedula *M3_sch_attuale;
	if(num_macchine == 3)
	{
		
		M3_sch_attuale = M3_sch;
	}

	array_job_attuale=malloc( num_job*sizeof ( job ) );
	insieme = NULL;
	insieme_temp=NULL;
	insieme_next=NULL;

	
	for(i = 0;i<num_job;i++)
	{
		array_job_attuale[i] = array_job[i];
	}
	//inizializzo insieme_temp perchè all'inizio del rollout deve contenere tutti i job
	for(i=0; i<num_job; i++){
		insieme=aggiungi_dinamicamente(insieme, array_job_attuale[i]);
	}

	job * perm_di_passaggio;

	//WHILE-STADI-ROLLOUT
	while(cont_livelli>0)
	{
		insieme_temp=insieme;	
		while(insieme_temp != NULL)
		{	
			/*considero tutti i job di insieme_temp schedulati per primi
			devo usare
			 costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
			per valutare l'inserimento di un job in una macchina
			*/
			if (insieme_temp->ID==-1)//se �non selezionabile
			{
				//il caso di avere un job non selezionabile in insieme_temp non esiste con questa implementazione basic del job choosing
			}
			else //se �selezionabile
			{	
				array_job_attuale_temp=malloc((cont_livelli)*sizeof(job));
				//metto in array_job_attuale_temp i job ancora da schedulare che mi servono per essere permutati
				iter=0;
				for(kk=0;kk<cont_livelli;kk++)
				{
					while(array_job_attuale[iter].ID==-1 || array_job_attuale[iter].ID== insieme_temp->ID)
					{
						iter++;
					}
					array_job_attuale_temp[kk]=array_job_attuale[iter];
					iter++;
				}
				next_elem *prossimo = NULL;
				for (i = 0;i<num_heur;i++)
				{
					//inizializza la perm_di_passaggio con la best_perm, cioè con la schedulazione già fatta
					azzera_schedule();
					perm_di_passaggio=malloc(num_job*sizeof(job));
					inizializza_perutazione_migliore(perm_di_passaggio); 
					for(pp=0;pp<num_job-cont_livelli-1;pp++)
					{
						perm_di_passaggio[pp]=best_perm[pp];
					}
					pp++;
					//mette nella perm di passaggio il job fisso
					perm_di_passaggio[(num_job-cont_livelli-1)].ID=insieme_temp->ID;
					perm_di_passaggio[(num_job-cont_livelli-1)].proc_time=insieme_temp->proc_time;
					perm_di_passaggio[(num_job-cont_livelli-1)].duedate=insieme_temp->duedate;
					perm_di_passaggio[(num_job-cont_livelli-1)].deadline=insieme_temp->deadline;
					perm_di_passaggio[(num_job-cont_livelli-1)].priority=insieme_temp->priority;
					perm_di_passaggio[(num_job-cont_livelli-1)].rel_time=insieme_temp->rel_time;

					next_elem *prossimo2;
					prossimo2= malloc(sizeof(next_elem));					prossimo2->next=NULL; 
					costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
					//prossimo2 contiene le informazioni su questa schedulazione con perm_di_passaggio
					free(prossimo2);
					azzera_schedule();
					permutazioni[i]=NULL;
					
					permutazioni[i] = funzioni[i].funz(array_job_attuale_temp,cont_livelli);
					
					//si scrive nelle righe di permutazioni la permutazione trovata dalla i-esima euristica
					//se prossimo è NULL si inizializza come testa della lista altrimenti si aggiunge in cosa un nuovo elemento temp
					//si aggiunge un next_elem col primo job della permutazione trovata dall'euristica e relativi dati della schedula parziale
					if(prossimo == NULL)
					{
						prossimo = malloc(sizeof(next_elem));
						prossimo->ID_job = insieme_temp->ID;
						prossimo->ID_heur= funzioni[i].ID_heur;
						prossimo->next = NULL;
						insieme_next=aggiungi_dinamicamente (insieme_next, permutazioni[i][0]);					
					}
					else
					{
						next_elem *temp;
						next_elem *temp_prox;
						temp = prossimo;						while(temp->next!=NULL)
						{
							temp = temp->next;
						}
						temp_prox=malloc(sizeof(next_elem));
						temp->next = temp_prox;
						temp = temp->next;
						temp->ID_job = insieme_temp->ID;
						temp->ID_heur= funzioni[i].ID_heur;
						temp->next = NULL;
						insieme_next=aggiungi_dinamicamente (insieme_next, permutazioni[i][0]);
		
					}
					//completo la fine perm_di_passaggio con la permutazione appena trovata dall'euristica
					for(jj=0;pp<num_job;pp++,jj++)
					{
						perm_di_passaggio[pp]=permutazioni[i][jj];
					}					
					azzera_schedule();
					//costruisce e valuta la schedula ottenuta dalla perm_di_passaggio salvando i risultati ottenuti in prossimo
					costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job);
					printf("  %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli);
					salva_se_meglio(perm_di_passaggio,best_perm,cont_livelli,iter_for);
					free(perm_di_passaggio);
				}//FINE for i<num_heur
				// se la schedula non �feasible deve essere penalizzata rispetto alle altre.
				// devo ridurre il numero di job che rimangono da schedulare
				// devo trovare il job con la Lateness + alta 
				// in condizioni di parit�quello con la Cmax +bassa
				// infine con il numero + basso di Tardy job

				free(array_job_attuale_temp);
				int ID_job = prossimo->ID_job;
				int macchina = prossimo->macchina;
				int fine = prossimo->fine;
				int inizio = prossimo->inizio;
				int tipo = prossimo->tipo;
				int index_camp = prossimo->index_camp;
				int LMAX = prossimo->Lmax_pers;
				int TARDY = prossimo->Tardy_pers;
				int L_max = prossimo->Lmax;
				int C_max = prossimo->Cmax;
				int tardy = prossimo->Tardy;
				int ID_heur = prossimo->ID_heur;
				int rel_time = prossimo->rel_time;
				int proc_time = prossimo->proc_time;
				int duedate = prossimo->duedate;
				int deadline = prossimo->deadline;
				int priority = prossimo->priority;
				Feasible = prossimo->feasible;		
				next_elem *temp = prossimo ;
				
				//WHILE-VINCITORI-EURISTICHE
				while(temp->next!=NULL)	
				{
					if (Feasible <= temp->next->feasible)		
						{
						if
						(
							(Feasible ==0)
							&&
							(temp->next->feasible==1)
						)
						{
							ID_job = temp->next->ID_job; 
							macchina = temp->next->macchina;
							tipo = temp->next->tipo;
							fine = temp->next->fine;
							inizio = temp->next->inizio;
							index_camp = temp->next->index_camp;
							LMAX = temp->next->Lmax_pers;
							TARDY = temp->next->Tardy_pers;
							L_max = temp->next->Lmax;
							C_max = temp->next->Cmax;
							tardy = temp->next->Tardy;				
							deadline = temp->next->deadline;
							duedate = temp->next->duedate;
							proc_time = temp->next->proc_time;
							rel_time = temp->next->rel_time;
							priority = temp->next->priority;
							ID_heur = temp->next->ID_heur;
							Feasible = temp->next->feasible;
						}
						else if
						(	
							(L_max > temp->next->Lmax)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max > temp->next->Cmax)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy > temp->next->Tardy)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&
								(LMAX > temp->next->Lmax_pers)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&
								(LMAX > temp->next->Lmax_pers)
								&&
								(
								(temp->next->duedate !=0)
								&&
								(duedate > temp->next->duedate)
								)					
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(
								(temp->next->deadline !=0)
								&&
								(deadline > temp->next->deadline)
								)						
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(deadline == temp->next->deadline)
								&&
								(proc_time > temp->next->proc_time)		
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(deadline == temp->next->deadline)
								&&
								(proc_time == temp->next->proc_time)
								&&
								(rel_time > temp->next->proc_time)		
							)	
						)		
						{
							ID_job = temp->next->ID_job; 
							macchina = temp->next->macchina;
							tipo = temp->next->tipo;
							fine = temp->next->fine;
							inizio = temp->next->inizio;
							index_camp = temp->next->index_camp;
							LMAX = temp->next->Lmax_pers;
							TARDY = temp->next->Tardy_pers;
							L_max = temp->next->Lmax;
							C_max = temp->next->Cmax;
							tardy = temp->next->Tardy;				
							deadline = temp->next->deadline;
							duedate = temp->next->duedate;
							proc_time = temp->next->proc_time;
							rel_time = temp->next->rel_time;
							priority = temp->next->priority;
							ID_heur = temp->next->ID_heur;
							Feasible = temp->next->feasible;
						}
						else{
							
						}				
					}
					else {
						
					}
					temp=temp->next;		
				}//FINE WHILE-VINCITORI-EURISTICHE:scorre temp (prossimo) salvando di volta in volta valori migliori tra tutte le schedule contenute in prossimo, che erano quelle trovate da tutte le euristiche tenendo questo job come fisso
				
			//ora sappiamo qual �L'ID del Job da eliminare dalla lista dei job da schedulare
			//e su quale macchina �stato schedulato	
 				printf("(%i) %i %i %i %i",cont_livelli, ID_heur, L_max, C_max,tardy);
				printf("\n");	
				printf("\n");			
				// salvataggio nella lista delle info sul job prescelto in questo step		
				if (lista_vincitori == NULL){
					lista_vincitori=malloc(sizeof(next_elem));
					lista_vincitori->ID_job = ID_job; 
					lista_vincitori->macchina = macchina;
					lista_vincitori->tipo =  tipo;
					lista_vincitori->fine = fine;
					lista_vincitori->inizio = inizio;
					lista_vincitori->index_camp = index_camp;
					lista_vincitori->Lmax_pers = LMAX;
					lista_vincitori->Tardy_pers =TARDY;
					lista_vincitori->Lmax = L_max;
					lista_vincitori->Cmax = C_max;
					lista_vincitori->Tardy = tardy;	
					lista_vincitori->deadline = deadline;
					lista_vincitori->duedate = duedate;
					lista_vincitori->proc_time = proc_time;
					lista_vincitori->rel_time = rel_time;
					lista_vincitori->priority = priority;
					lista_vincitori->ID_heur = ID_heur;
					lista_vincitori->feasible = Feasible;
					lista_vincitori->next=NULL;
				}
				else{

					next_elem *vincitori_temp;
					next_elem *vincitori_temp_prox;
					vincitori_temp = lista_vincitori;
					while(vincitori_temp->next!=NULL)
					{
						vincitori_temp = vincitori_temp->next;
					}
					vincitori_temp_prox=malloc(sizeof(next_elem));
					vincitori_temp->next = vincitori_temp_prox;
					vincitori_temp = vincitori_temp->next;
					vincitori_temp->ID_job = ID_job; 
					vincitori_temp->macchina = macchina;
					vincitori_temp->tipo =  tipo;
					vincitori_temp->fine = fine;
					vincitori_temp->inizio = inizio;
					vincitori_temp->index_camp = index_camp;
					vincitori_temp->Lmax_pers = LMAX;
					vincitori_temp->Tardy_pers =TARDY;
					vincitori_temp->Lmax = L_max;
					vincitori_temp->Cmax = C_max;
					vincitori_temp->Tardy = tardy;	
					vincitori_temp->deadline = deadline;
					vincitori_temp->duedate = duedate;
					vincitori_temp->proc_time = proc_time;
					vincitori_temp->rel_time = rel_time;
					vincitori_temp->priority = priority;
					vincitori_temp->ID_heur = ID_heur;
					vincitori_temp->feasible = Feasible;
					vincitori_temp->next = NULL;
				}	
				// fine salvataggio 

				//libero prossimo e permutazioni per il prossimo ciclo
				next_elem *temp_pr;
				
				while (prossimo->next != NULL){
					temp_pr = prossimo->next;
					free(prossimo);
					prossimo = temp_pr;
				}
				free(prossimo);
				prossimo=NULL;
	
				for(i=0;i<num_heur;i++)
				{
						free(permutazioni[i]);
				}
				

			}
			insieme_temp=insieme_temp->next;
			
		} //FINE while(insieme_temp->testa != NULL) (quello che fa partire il rollout da tutti i job)
 		printf(" %i____________________\n\n\n\n",cont_livelli);				
		
		int ID_job = lista_vincitori->ID_job;
		Feasible = lista_vincitori->feasible;
		next_elem *migliore=lista_vincitori;
		next_elem *vincitori_temp=lista_vincitori;	
		while(vincitori_temp->next != NULL){
			if (Feasible <= vincitori_temp->next->feasible)		
				{
				if
				(
					(Feasible ==0)
					&&
					(vincitori_temp->next->feasible==1)
				)
				{
					migliore=vincitori_temp;
					ID_job = vincitori_temp->next->ID_job; 
					Feasible = vincitori_temp->next->feasible;
				}
				else if
				(	
					(migliore->Lmax > vincitori_temp->next->Lmax)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax > vincitori_temp->next->Cmax)
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy > vincitori_temp->next->Tardy)
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(
						(vincitori_temp->next->duedate !=0)
						&&
						(migliore->duedate > vincitori_temp->next->duedate)
						)					
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&						
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(migliore->duedate == vincitori_temp->next->duedate)
						&&
						(
						(vincitori_temp->next->deadline !=0)
						&&
						(migliore->deadline > vincitori_temp->next->deadline)
						)						
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&						
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(migliore->duedate == vincitori_temp->next->duedate)
						&&
						(migliore->deadline == vincitori_temp->next->deadline)
						&&
						(migliore->proc_time > vincitori_temp->next->proc_time)		
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&						
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(migliore->duedate == vincitori_temp->next->duedate)
						&&
						(migliore->deadline == vincitori_temp->next->deadline)
						&&
						(migliore->proc_time == vincitori_temp->next->proc_time)
						&&
						(migliore->rel_time > vincitori_temp->next->proc_time)		
					)	
				)		
				{
					migliore=vincitori_temp;
					ID_job = vincitori_temp->next->ID_job; 
					Feasible = vincitori_temp->next->feasible;
				}
				else{
					
				}				
			}
			else {
				
			}
			vincitori_temp=vincitori_temp->next;		
		}
		
		// devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
		//poi inserisco in best_perm il job migliore fra tutti i vincitori dello stadio di rollout
		if
		(
			(migliore->feasible>Feasible_best)
		||
			(
				(migliore->feasible==Feasible_best)
				&&
				(migliore->Lmax<Lmax_best)
			)
		||
			(
				(migliore->feasible==Feasible_best)
				&&
				(migliore->Lmax==Lmax_best)
				&&
				(migliore->Cmax<Cmax_best)
			)
		||
			(
				(migliore->feasible==Feasible_best)
				&&
				(migliore->Lmax==Lmax_best)
				&&
				(migliore->Cmax==Cmax_best)
				&&
				(migliore->Tardy<Tardy_best)
			)
		)
		{
			best_perm[num_job-cont_livelli-1].ID=array_job_attuale[ID_job].ID;
			best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[ID_job].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[ID_job].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[ID_job].duedate;
			best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[ID_job].deadline;
			best_perm[num_job-cont_livelli-1].priority=array_job_attuale[ID_job].priority;
			best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[ID_job].rel_time;
			insieme=copia_dinamicamente(insieme, insieme_next, array_job_attuale[ID_job]);
			//il job schedulato viene segnato come non selezionabile (non rischedulabile)
			array_job_attuale[ID_job].ID=-1;
		}		
		else
		{
			best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
			best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
			best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
			best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
			best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
			insieme=copia_dinamicamente(insieme, insieme_next, permutazione_buffer[num_job-cont_livelli-1]);
			//il job schedulato viene segnato come non selezionabile (non rischedulabile)
			array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;;
			
		}
		//se insieme resta vuoto (coincide l'unico job di insieme_next col successivo della permutazione_buffer)
		if (insieme == NULL){
			insieme=aggiungi_dinamicamente(insieme, array_job_attuale[ID_job]);
		}
		insieme_next=NULL;
		while (lista_vincitori->next != NULL){
			vincitori_temp = lista_vincitori;
			lista_vincitori=lista_vincitori->next;
			free(vincitori_temp);
		}	
		free(lista_vincitori);
		lista_vincitori=NULL;
		cont_livelli--;      
	} //FINE WHILE-STADI-ROLLOUT
	
	//devo schedulare l'ultimo job
	i=0;
	while(array_job_attuale[i].ID==-1)
	{
		i++;
	}
	best_perm[num_job-1].ID=array_job_attuale[i].ID;
	best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
	best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
	best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
	best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
	best_perm[num_job-1].priority=array_job_attuale[i].priority;
	best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
	array_job_attuale[i].ID=-1;
	//libero memoria che non mi serve più
	free(array_job_attuale);
	azzera_schedule();
	next_elem *prossimo1;
 	prossimo1= malloc(sizeof(next_elem));
	prossimo1->next=NULL; 
	//costruisci e valuta la permutazione finale, confronto con quella buffer e restituisco la permutazione trovata dall'intero rollout
	costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job);
	free(prossimo1);	
	if
	(
		(Feasible<Feasible_best)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax>Lmax_best)
		)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax==Lmax_best)
			&&
			(Cmax>Cmax_best)
		)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax==Lmax_best)
			&&
			(Cmax==Cmax_best)
			&&
			(Tardy>Tardy_best)
		)
	)
	{
		free(permutazioni);
		return permutazione_buffer;
	}		
	else
	{
		free(permutazioni);
		free(permutazione_buffer);
		return	best_perm;
	}
}


/*
Esegue il rollout col dynamic job choosing e l'heuristic pruning allo stesso tempo.
Restituisce una permutazione dei job
*/
job *rollout_dynamic ( int force,char *instance_file )
{
// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo
	int i,iter,kk  = 0;
	int pp=0,jj=0;
	int iter_for = 0;
	int cont_livelli= num_job-1;
	int num_job_relativo = num_job;	
	int j = 0;
	job **permutazioni;
	permutazioni= malloc(num_heur*sizeof(job *));	
	job *job_da_inserire;
	int primo_passo_M1 = 0;
	int primo_passo_M2 = 0;
	int primo_passo_M3 = 0;
	FILE *f_log;
	char nome_file[25];
	insieme_dinamico *insieme, *insieme_next, *insieme_temp;
	//non ho fatto ancora nessuna assegnazione alla macchina 3 se vale 0
	job *array_job_attuale;
	job *array_job_attuale_temp;
	schedula *M1_sch_attuale;
	schedula *M1_sch_buffer;
	schedula *M2_sch_buffer;
	schedula *M3_sch_buffer;
	schedula *M1_sch_buffer1;
	schedula *M2_sch_buffer1;
	schedula *M3_sch_buffer1;
	schedula **vettore_schedule_rollout;
	schedula *punt_vett_roll;
	next_elem *lista_vincitori=NULL; //contiene i vincitori dei singoli turni dello stadio di rollout
	M1_sch_attuale = M1_sch;
	schedula *temp1;
	schedula *temp2;
	int fine = 0;
 	permutazione_buffer=malloc(num_job*sizeof(job));
	schedula *M2_sch_attuale; //puntatore all'ultimo elemento della schedula in costruzione su M2
	if(num_macchine >= 2)
	{
		M2_sch_attuale = M2_sch;
	}
	schedula *M3_sch_attuale;
	if(num_macchine == 3)
	{
		
		M3_sch_attuale = M3_sch;
	}

	array_job_attuale=malloc( num_job*sizeof ( job ) );
	insieme = NULL;
	insieme_temp=NULL;
	insieme_next=NULL;

	
	for(i = 0;i<num_job;i++)
	{
		array_job_attuale[i] = array_job[i];
	}
	//inizializzo insieme_temp perchè all'inizio del rollout deve contenere tutti i job
	for(i=0; i<num_job; i++){
		insieme=aggiungi_dinamicamente(insieme, array_job_attuale[i]);
	}

	job * perm_di_passaggio;

	//WHILE-STADI-ROLLOUT
	while(cont_livelli>0)
	{
		insieme_temp=insieme;	
		while(insieme_temp != NULL)
		{	
			/*considero tutti i job di insieme_temp schedulati per primi
			devo usare
			 costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
			per valutare l'inserimento di un job in una macchina
			*/
			if (insieme_temp->ID==-1)//se �non selezionabile
			{
			}
			else //se �selezionabile
			{	
				array_job_attuale_temp=malloc((cont_livelli)*sizeof(job));
				//metto in array_job_attuale_temp i job ancora da schedulare che mi servono per essere permutati
				iter=0;
				for(kk=0;kk<cont_livelli;kk++)
				{
					while(array_job_attuale[iter].ID==-1 || array_job_attuale[iter].ID== insieme_temp->ID)
					{
						iter++;
					}
					array_job_attuale_temp[kk]=array_job_attuale[iter];
					iter++;
				}
				next_elem *prossimo = NULL;
				for (i = 0;i<num_heur;i++)
				{
					if(funzioni[i].perc_utilizzo !=-1){
						//inizializza la perm_di_passaggio con la best_perm, cioè con la schedulazione già fatta
						azzera_schedule();
						perm_di_passaggio=malloc(num_job*sizeof(job));
						inizializza_perutazione_migliore(perm_di_passaggio); 
						for(pp=0;pp<num_job-cont_livelli-1;pp++)
						{
							perm_di_passaggio[pp]=best_perm[pp];
						}
						pp++;
						//mette nella perm di passaggio il job fisso
						perm_di_passaggio[(num_job-cont_livelli-1)].ID=insieme_temp->ID;
						perm_di_passaggio[(num_job-cont_livelli-1)].proc_time=insieme_temp->proc_time;
						perm_di_passaggio[(num_job-cont_livelli-1)].duedate=insieme_temp->duedate;
						perm_di_passaggio[(num_job-cont_livelli-1)].deadline=insieme_temp->deadline;
						perm_di_passaggio[(num_job-cont_livelli-1)].priority=insieme_temp->priority;
						perm_di_passaggio[(num_job-cont_livelli-1)].rel_time=insieme_temp->rel_time;
	
						next_elem *prossimo2;
						prossimo2= malloc(sizeof(next_elem));					prossimo2->next=NULL; 
						costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo2,perm_di_passaggio,num_job);
						//prossimo2 contiene le informazioni su questa schedulazione con perm_di_passaggio
						free(prossimo2);
						azzera_schedule();
						permutazioni[i]=NULL;
						
						permutazioni[i] = funzioni[i].funz(array_job_attuale_temp,cont_livelli);
						
						//si scrive nelle righe di permutazioni la permutazione trovata dalla i-esima euristica
						//se prossimo è NULL si inizializza come testa della lista altrimenti si aggiunge in cosa un nuovo elemento temp
						//si aggiunge un next_elem col primo job della permutazione trovata dall'euristica e relativi dati della schedula parziale
						if(prossimo == NULL)
						{
							prossimo = malloc(sizeof(next_elem));
							prossimo->ID_job = insieme_temp->ID;
							prossimo->ID_heur= funzioni[i].ID_heur;
							prossimo->next = NULL;
							insieme_next=aggiungi_dinamicamente (insieme_next, permutazioni[i][0]);					
						}
						else
						{
							next_elem *temp;
							next_elem *temp_prox;
							temp = prossimo;						while(temp->next!=NULL)
							{
								temp = temp->next;
							}
							temp_prox=malloc(sizeof(next_elem));
							temp->next = temp_prox;
							temp = temp->next;
							temp->ID_job = insieme_temp->ID;
							temp->ID_heur= funzioni[i].ID_heur;
							temp->next = NULL;
							insieme_next=aggiungi_dinamicamente (insieme_next, permutazioni[i][0]);
			
						}
						//completo la fine perm_di_passaggio con la permutazione appena trovata dall'euristica
						for(jj=0;pp<num_job;pp++,jj++)
						{
							perm_di_passaggio[pp]=permutazioni[i][jj];
						}					
						azzera_schedule();
						//costruisce e valuta la schedula ottenuta dalla perm_di_passaggio salvando i risultati ottenuti in prossimo
						costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job);
						printf("  %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli);
						salva_se_meglio(perm_di_passaggio,best_perm,cont_livelli,iter_for);
						free(perm_di_passaggio);
					}
					else{
						//se la euristica è stata esclusa in precedenza...
					}
				}//FINE for i<num_heur
				// se la schedula non �feasible deve essere penalizzata rispetto alle altre.
				// devo ridurre il numero di job che rimangono da schedulare
				// devo trovare il job con la Lateness + alta 
				// in condizioni di parit�quello con la Cmax +bassa
				// infine con il numero + basso di Tardy job

				free(array_job_attuale_temp);
				int ID_job = prossimo->ID_job;
				int macchina = prossimo->macchina;
				int fine = prossimo->fine;
				int inizio = prossimo->inizio;
				int tipo = prossimo->tipo;
				int index_camp = prossimo->index_camp;
				int LMAX = prossimo->Lmax_pers;
				int TARDY = prossimo->Tardy_pers;
				int L_max = prossimo->Lmax;
				int C_max = prossimo->Cmax;
				int tardy = prossimo->Tardy;
				int ID_heur = prossimo->ID_heur;
				int rel_time = prossimo->rel_time;
				int proc_time = prossimo->proc_time;
				int duedate = prossimo->duedate;
				int deadline = prossimo->deadline;
				int priority = prossimo->priority;
				Feasible = prossimo->feasible;		
				next_elem *temp = prossimo ;
				
				int c=1; //variabile che conta per sapere i vincitori locali
				int vinc_loc=ID_heur; //variabile che tiene traccia del vincitore locale;
				if (c==num_heur_utilizz){
					funzioni[vinc_loc].perc_utilizzo++;
					printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
				}

				//WHILE-VINCITORI-EURISTICHE
				while(temp->next!=NULL)	
				{
					if (Feasible <= temp->next->feasible)		
						{
						if
						(
							(Feasible ==0)
							&&
							(temp->next->feasible==1)
						)
						{
							ID_job = temp->next->ID_job; 
							macchina = temp->next->macchina;
							tipo = temp->next->tipo;
							fine = temp->next->fine;
							inizio = temp->next->inizio;
							index_camp = temp->next->index_camp;
							LMAX = temp->next->Lmax_pers;
							TARDY = temp->next->Tardy_pers;
							L_max = temp->next->Lmax;
							C_max = temp->next->Cmax;
							tardy = temp->next->Tardy;				
							deadline = temp->next->deadline;
							duedate = temp->next->duedate;
							proc_time = temp->next->proc_time;
							rel_time = temp->next->rel_time;
							priority = temp->next->priority;
							ID_heur = temp->next->ID_heur;
							Feasible = temp->next->feasible;
							c++;
							vinc_loc=ID_heur;
							if (c==num_heur_utilizz){
								funzioni[vinc_loc].perc_utilizzo++;
								printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
								c=0;
							}
						}
						else if
						(	
							(L_max > temp->next->Lmax)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max > temp->next->Cmax)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy > temp->next->Tardy)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&
								(LMAX > temp->next->Lmax_pers)
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&
								(LMAX > temp->next->Lmax_pers)
								&&
								(
								(temp->next->duedate !=0)
								&&
								(duedate > temp->next->duedate)
								)					
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(
								(temp->next->deadline !=0)
								&&
								(deadline > temp->next->deadline)
								)						
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(deadline == temp->next->deadline)
								&&
								(proc_time > temp->next->proc_time)		
							)
							||
							(
								(L_max == temp->next->Lmax)
								&&
								(C_max == temp->next->Cmax)
								&&
								(tardy == temp->next->Tardy)
								&&						
								(LMAX > temp->next->Lmax_pers)
								&&
								(duedate == temp->next->duedate)
								&&
								(deadline == temp->next->deadline)
								&&
								(proc_time == temp->next->proc_time)
								&&
								(rel_time > temp->next->proc_time)		
							)	
						)		
						{
							ID_job = temp->next->ID_job; 
							macchina = temp->next->macchina;
							tipo = temp->next->tipo;
							fine = temp->next->fine;
							inizio = temp->next->inizio;
							index_camp = temp->next->index_camp;
							LMAX = temp->next->Lmax_pers;
							TARDY = temp->next->Tardy_pers;
							L_max = temp->next->Lmax;
							C_max = temp->next->Cmax;
							tardy = temp->next->Tardy;				
							deadline = temp->next->deadline;
							duedate = temp->next->duedate;
							proc_time = temp->next->proc_time;
							rel_time = temp->next->rel_time;
							priority = temp->next->priority;
							ID_heur = temp->next->ID_heur;
							Feasible = temp->next->feasible;
							c++;
							vinc_loc=ID_heur;
							if (c==num_heur_utilizz){
								funzioni[vinc_loc].perc_utilizzo++;
								printf("La funzione %i vince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
								c=0;
							}
						}
						else{
							if (c!=num_heur_utilizz)
								c++;
							else {
								c=1;
								//funzioni[vinc_loc].perc_utilizzo++;
								printf("La funzione %i rivince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
								//se tutte le euristiche del turno non portano benefici si considera vincitore l'ultimo che è stato
							}			
						}				
					}
					else {
						if (c!=num_heur_utilizz)
							c++;
						else {
							c=1;
							//funzioni[vinc_loc].perc_utilizzo++;
							printf("La funzione %i rivince e la sua perc è: %i",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
							//se tutte le euristiche del turno sono non feasible si considera vincitore l'ultimo che è stato
						}
					}
					temp=temp->next;		
				}//FINE WHILE-VINCITORI-EURISTICHE:scorre temp (prossimo) salvando di volta in volta valori migliori tra tutte le schedule contenute in prossimo, che erano quelle trovate da tutte le euristiche tenendo questo job come fisso
				
				if (c==num_heur_utilizz && num_heur_utilizz !=1){
					funzioni[vinc_loc].perc_utilizzo++;
					printf("La funzione %i vince e la sua perc è: %i \n",vinc_loc,funzioni[vinc_loc].perc_utilizzo);
				}

				//ora sappiamo qual �L'ID del Job da eliminare dalla lista dei job da schedulare
				// e su quale macchina �stato schedulato	

				printf("(%i) %i %i %i %i",cont_livelli, ID_heur, L_max, C_max,tardy);
				printf("\n");	
				printf("\n");			
				// salvataggio nella lista delle info sul job prescelto in questo step		
				if (lista_vincitori == NULL){
					lista_vincitori=malloc(sizeof(next_elem));
					lista_vincitori->ID_job = ID_job; 
					lista_vincitori->macchina = macchina;
					lista_vincitori->tipo =  tipo;
					lista_vincitori->fine = fine;
					lista_vincitori->inizio = inizio;
					lista_vincitori->index_camp = index_camp;
					lista_vincitori->Lmax_pers = LMAX;
					lista_vincitori->Tardy_pers =TARDY;
					lista_vincitori->Lmax = L_max;
					lista_vincitori->Cmax = C_max;
					lista_vincitori->Tardy = tardy;	
					lista_vincitori->deadline = deadline;
					lista_vincitori->duedate = duedate;
					lista_vincitori->proc_time = proc_time;
					lista_vincitori->rel_time = rel_time;
					lista_vincitori->priority = priority;
					lista_vincitori->ID_heur = ID_heur;
					lista_vincitori->feasible = Feasible;
					lista_vincitori->next=NULL;
				}
				else{
					next_elem *vincitori_temp;
					next_elem *vincitori_temp_prox;
					vincitori_temp = lista_vincitori;
					while(vincitori_temp->next!=NULL)
					{
						vincitori_temp = vincitori_temp->next;
					}
					vincitori_temp_prox=malloc(sizeof(next_elem));
					vincitori_temp->next = vincitori_temp_prox;
					vincitori_temp = vincitori_temp->next;
					vincitori_temp->ID_job = ID_job; 
					vincitori_temp->macchina = macchina;
					vincitori_temp->tipo =  tipo;
					vincitori_temp->fine = fine;
					vincitori_temp->inizio = inizio;
					vincitori_temp->index_camp = index_camp;
					vincitori_temp->Lmax_pers = LMAX;
					vincitori_temp->Tardy_pers =TARDY;
					vincitori_temp->Lmax = L_max;
					vincitori_temp->Cmax = C_max;
					vincitori_temp->Tardy = tardy;	
					vincitori_temp->deadline = deadline;
					vincitori_temp->duedate = duedate;
					vincitori_temp->proc_time = proc_time;
					vincitori_temp->rel_time = rel_time;
					vincitori_temp->priority = priority;
					vincitori_temp->ID_heur = ID_heur;
					vincitori_temp->feasible = Feasible;
					vincitori_temp->next = NULL;
				}	
				// fine salvataggio 

				//libero prossimo e permutazioni per il prossimo ciclo
				next_elem *temp_pr;
				
				while (prossimo->next != NULL){
					temp_pr = prossimo->next;
					free(prossimo);
					prossimo = temp_pr;
				}
				free(prossimo);
				prossimo=NULL;
	
				for(i=0;i<num_heur;i++)
				{
					if (funzioni[i].perc_utilizzo != -1)
						free(permutazioni[i]);
				}
				

			}
			insieme_temp=insieme_temp->next;
			
		} //FINE while(insieme_temp->testa != NULL) (quello che fa partire il rollout da tutti i job)
 		printf(" %i____________________\n\n\n\n",cont_livelli);				
		
		int ID_job = lista_vincitori->ID_job;
		Feasible = lista_vincitori->feasible;
		next_elem *migliore=lista_vincitori;
		next_elem *vincitori_temp=lista_vincitori;	
		while(vincitori_temp->next != NULL){
			if (Feasible <= vincitori_temp->next->feasible)		
				{
				if
				(
					(Feasible ==0)
					&&
					(vincitori_temp->next->feasible==1)
				)
				{
					migliore=vincitori_temp;
					ID_job = vincitori_temp->next->ID_job; 
					Feasible = vincitori_temp->next->feasible;
				}
				else if
				(	
					(migliore->Lmax > vincitori_temp->next->Lmax)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax > vincitori_temp->next->Cmax)
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy > vincitori_temp->next->Tardy)
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(
						(vincitori_temp->next->duedate !=0)
						&&
						(migliore->duedate > vincitori_temp->next->duedate)
						)					
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&						
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(migliore->duedate == vincitori_temp->next->duedate)
						&&
						(
						(vincitori_temp->next->deadline !=0)
						&&
						(migliore->deadline > vincitori_temp->next->deadline)
						)						
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&						
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(migliore->duedate == vincitori_temp->next->duedate)
						&&
						(migliore->deadline == vincitori_temp->next->deadline)
						&&
						(migliore->proc_time > vincitori_temp->next->proc_time)		
					)
					||
					(
						(migliore->Lmax == vincitori_temp->next->Lmax)
						&&
						(migliore->Cmax == vincitori_temp->next->Cmax)
						&&
						(migliore->Tardy == vincitori_temp->next->Tardy)
						&&						
						(migliore->Lmax_pers > vincitori_temp->next->Lmax_pers)
						&&
						(migliore->duedate == vincitori_temp->next->duedate)
						&&
						(migliore->deadline == vincitori_temp->next->deadline)
						&&
						(migliore->proc_time == vincitori_temp->next->proc_time)
						&&
						(migliore->rel_time > vincitori_temp->next->proc_time)		
					)	
				)		
				{
					migliore=vincitori_temp;
					ID_job = vincitori_temp->next->ID_job; 
					Feasible = vincitori_temp->next->feasible;
				}
				else{
					
				}				
			}
			else {
	
			}
			vincitori_temp=vincitori_temp->next;		
		}
		
		// devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
		//poi inserisco in best_perm il job migliore fra tutti i vincitori dello stadio di rollout
		if
		(
			(migliore->feasible>Feasible_best)
		||
			(
				(migliore->feasible==Feasible_best)
				&&
				(migliore->Lmax<Lmax_best)
			)
		||
			(
				(migliore->feasible==Feasible_best)
				&&
				(migliore->Lmax==Lmax_best)
				&&
				(migliore->Cmax<Cmax_best)
			)
		||
			(
				(migliore->feasible==Feasible_best)
				&&
				(migliore->Lmax==Lmax_best)
				&&
				(migliore->Cmax==Cmax_best)
				&&
				(migliore->Tardy<Tardy_best)
			)
		)
		{
			best_perm[num_job-cont_livelli-1].ID=array_job_attuale[ID_job].ID;
			best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[ID_job].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[ID_job].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[ID_job].duedate;
			best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[ID_job].deadline;
			best_perm[num_job-cont_livelli-1].priority=array_job_attuale[ID_job].priority;
			best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[ID_job].rel_time;
			insieme=copia_dinamicamente(insieme, insieme_next, array_job_attuale[ID_job]);
			//il job schedulato viene segnato come non selezionabile (non rischedulabile)
			array_job_attuale[ID_job].ID=-1;
		}		
		else
		{
			best_perm[num_job-cont_livelli-1].ID = permutazione_buffer[num_job-cont_livelli-1].ID;
			best_perm[num_job-cont_livelli-1].tipo=permutazione_buffer[num_job-cont_livelli-1].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=permutazione_buffer[num_job-cont_livelli-1].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=permutazione_buffer[num_job-cont_livelli-1].duedate;
			best_perm[num_job-cont_livelli-1].deadline=permutazione_buffer[num_job-cont_livelli-1].deadline;
			best_perm[num_job-cont_livelli-1].priority=permutazione_buffer[num_job-cont_livelli-1].priority;
			best_perm[num_job-cont_livelli-1].rel_time=permutazione_buffer[num_job-cont_livelli-1].rel_time;
			insieme=copia_dinamicamente(insieme, insieme_next, permutazione_buffer[num_job-cont_livelli-1]);
			//il job schedulato viene segnato come non selezionabile (non rischedulabile)
			array_job_attuale[permutazione_buffer[num_job-cont_livelli-1].ID].ID=-1;;
			
		}
		//se insieme resta vuoto (coincide l'unico job di insieme_next col successivo della permutazione_buffer)
		if (insieme == NULL){
			insieme=aggiungi_dinamicamente(insieme, array_job_attuale[ID_job]);
		}
		insieme_next=NULL;
		while (lista_vincitori->next != NULL){
			vincitori_temp = lista_vincitori;
			lista_vincitori=lista_vincitori->next;
			free(vincitori_temp);
		}	
		free(lista_vincitori);
		lista_vincitori=NULL;
		escludi_euristica(funzioni,politica_pruning,cont_livelli);
		cont_livelli--;      
	} //FINE WHILE-STADI-ROLLOUT
	
	//devo schedulare l'ultimo job
	i=0;
	while(array_job_attuale[i].ID==-1)
	{
		i++;
	}
	best_perm[num_job-1].ID=array_job_attuale[i].ID;
	best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
	best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
	best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
	best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
	best_perm[num_job-1].priority=array_job_attuale[i].priority;
	best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
	array_job_attuale[i].ID=-1;
	//libero memoria che non mi serve più
	free(array_job_attuale);
	azzera_schedule();
	next_elem *prossimo1;
 	prossimo1= malloc(sizeof(next_elem));
	prossimo1->next=NULL; 
	//costruisci e valuta la permutazione finale, confronto con quella buffer e restituisco la permutazione trovata dall'intero rollout
	costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo1,best_perm,num_job);
	free(prossimo1);	
	if
	(
		(Feasible<Feasible_best)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax>Lmax_best)
		)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax==Lmax_best)
			&&
			(Cmax>Cmax_best)
		)
	||
		(
			(Feasible==Feasible_best)
			&&
			(Lmax==Lmax_best)
			&&
			(Cmax==Cmax_best)
			&&
			(Tardy>Tardy_best)
		)
	)
	{
		free(permutazioni);
		return permutazione_buffer;
	}		
	else
	{
		free(permutazioni);
		free(permutazione_buffer);
		return	best_perm;
	}
}

/*Funzione che stampa i valori di output dividendoli in file di testo diversi; i quali sono a loro volta suddivisi in righe di 15 valori separati da blank. */
void stampa_su_file_antonio ( char *output_file, int flag )
{
	/*
	char *out_Lmax, *out_Cmax, *out_Tardy, *out_Feas, *out_Tempo;
	char str_temp[100];
	FILE *f_out_Lmax, *f_out_Cmax, *f_out_Tardy, *f_out_Feas, *f_out_Tempo;
	*/
	int i, temp;
	double temp1;


	if(flag == 0){	

		i=0;
		do{	
			i=fscanf(f_out_Cmax, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

			fscanf(f_out_Lmax, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

			fscanf(f_out_Tardy,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

			fscanf(f_out_Tempo,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1);

			fscanf(f_out_Feas,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);
		}
		while(i==15);
		if(i==14){
			fprintf (f_out_Lmax,"%d",Lmax);
			fprintf(f_out_Lmax,"\n");
			fprintf(f_out_Cmax,"%d",Cmax);
			fprintf(f_out_Cmax,"\n");
			fprintf(f_out_Tardy,"%d",Tardy);
			fprintf(f_out_Tardy,"\n");
			fprintf(f_out_Feas,"%d",Feasible);
			fprintf(f_out_Feas,"\n");
			fprintf(f_out_Tempo,"%.3f",(double) (tempo_fine1-tempo_inizio1)/CLOCKS_PER_SEC);
			fprintf(f_out_Tempo,"\n");
		}
		else if(i<14){
			fprintf (f_out_Lmax,"%d ",Lmax);
			fprintf(f_out_Cmax,"%d ",Cmax);
			fprintf(f_out_Tardy,"%d ",Tardy);
			fprintf(f_out_Feas,"%d ",Feasible);
			fprintf(f_out_Tempo,"%.3f ",(double) (tempo_fine1-tempo_inizio1)/CLOCKS_PER_SEC);
		}
		fclose(f_out_Lmax);
		fclose(f_out_Cmax);
		fclose(f_out_Tardy);
		fclose(f_out_Feas);
		fclose(f_out_Tempo);
	}
	else if(flag == 1){ //ho fatto una local search quindi misuro i tempi di rollout+local_search	

		i=0;
		do{	
			i=fscanf(f_out_Cmax, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

			fscanf(f_out_Lmax, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

			fscanf(f_out_Tardy,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

			fscanf(f_out_Tempo,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1,&temp1);

			fscanf(f_out_Feas,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);
		}
		while(i==15);
		if(i==14){
			fprintf (f_out_Lmax,"%d",Lmax);
			fprintf(f_out_Lmax,"\n");
			fprintf(f_out_Cmax,"%d",Cmax);
			fprintf(f_out_Cmax,"\n");
			fprintf(f_out_Tardy,"%d",Tardy);
			fprintf(f_out_Tardy,"\n");
			fprintf(f_out_Feas,"%d",Feasible);
			fprintf(f_out_Feas,"\n");
			fprintf(f_out_Tempo,"%.3f",( double ) ( tempo_fine2-tempo_inizio2 ) /CLOCKS_PER_SEC);
			fprintf(f_out_Tempo,"\n");
		}
		else if(i<14){
			fprintf (f_out_Lmax,"%d ",Lmax);
			fprintf(f_out_Cmax,"%d ",Cmax);
			fprintf(f_out_Tardy,"%d ",Tardy);
			fprintf(f_out_Feas,"%d ",Feasible);
			fprintf(f_out_Tempo,"%.3f ",( double ) ( tempo_fine2-tempo_inizio2 ) /CLOCKS_PER_SEC);
		}
		fclose(f_out_Lmax);
		fclose(f_out_Cmax);
		fclose(f_out_Tardy);
		fclose(f_out_Feas);
		fclose(f_out_Tempo);
	}
}
