#include "rollout.h"





job *rollout_time(int force,char *instance_file,int Tempo)
{




// M1_sch, M2_sch,M3_sch conterranno la schedula costruita passo dopo passo
// i due vettori che inizializzan e tengono conto del tempo corrente
 	time_t *diinizio,*corrente;
	next_elem *job_fisso ;
	int i,iter,kk,diff  = 0;
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
	job *job_da_inserire;
	int primo_passo_M1 = 0;
	int primo_passo_M2 = 0;
	int primo_passo_M3 = 0;
	int T=Tempo;
   	FILE *f_log;
   	char nome_file[25];
//    strcpy(nome_file,instance_file);
//    strcat(nome_file,".roll.txt");
//    f_log=fopen(nome_file,"a+");
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
	schedula *M2_sch_attuale;//puntatore all'ultimo elemento della schedula in costruzione su M2
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
// 		printf("%i ",array_job_attuale[i].ID);
	}
// 	printf("\n");
	job_fisso = (next_elem *)malloc(sizeof(next_elem));
// 	printf("\n");
        time(diinizio);
	printf("inizio \n");
	job * perm_di_passaggio;
	while(cont_livelli>0)
	{
		time(corrente);
		diff=difftime(*corrente,*diinizio);
		printf("sono passsati %d \n",diff);
		if (diff<Tempo){
		printf("il tempo e scaduto%d  \n");
		break;
		
		
		}
		next_elem lista_prossimi_vincitori[500];
		index=0;
		index1=0;
		/*devo creare un vettore contenente una copia di ciscun insieme di schedule [1 num_job_relativo]*/		
		for(iter_for=0;iter_for<num_job;iter_for++)
		{	
			/* di volta in volta ridurro il numero di num_job_relativo
			devo considerare il caso di tutti i job scedulati per primi
			devo usare costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,permutazioni[i],num_job-iter);
			tale funzione mi permette di valutare l'inseriemnto di un job in una macchina  */
			if (array_job_attuale[iter_for].ID==-1)//se �non selezionabile
			{
			}
			else //se �selezionabile
			{	
				job_fisso->ID_job = array_job_attuale[iter_for].ID;
				job_da_inserire = (job *)malloc(sizeof(job));// da eliminare 
				job_da_inserire[0] = array_job_attuale[iter_for];
				array_job_attuale[iter_for].ID =-1;
				array_job_attuale_temp=malloc((cont_livelli)*sizeof(job));
				iter=0;
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
				free(job_da_inserire);		
				next_elem *prossimo = NULL;
				next_elem *prossimo1;
				for (i = 0;i<num_heur;i++)//num_heur dovr�essere cambiato con il numero di heuristiche effettivamente usato
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
					free(prossimo2);
					azzera_schedule();
					permutazioni[i]=NULL;				
					permutazioni[i] = funzioni[i].funz(array_job_attuale_temp,cont_livelli);
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
					for(jj=0;pp<num_job;pp++,jj++)
					{
						perm_di_passaggio[pp]=permutazioni[i][jj];
					}					
					azzera_schedule();
					costruisci_e_valuta_schedula(M1_sch,M2_sch,M3_sch,prossimo,perm_di_passaggio,num_job);
					printf(" %i) %i %i %i %i num iter %i\n",i,Lmax,Cmax,Tardy,Feasible,num_job-cont_livelli);
 					salva_se_meglio(perm_di_passaggio,best_perm,cont_livelli,iter_for);
					free(perm_di_passaggio);
					//devo riportare la macchina nelle condizioni orginarie
// 					stampa_risultati_a_video(0); 
					if(force == 1)
					{					
						prossimo1 = malloc(sizeof(next_elem));
						M1_sch_buffer=malloc(sizeof(schedula));
						M2_sch_buffer=malloc(sizeof(schedula));
						M3_sch_buffer=malloc(sizeof(schedula));
						copia_schelule(M1_sch,M1_sch_buffer);
						if(num_macchine>=2)
						{
							copia_schelule(M2_sch,M2_sch_buffer);
						}
						if(num_macchine==3)
						{
							copia_schelule(M3_sch,M3_sch_buffer);
						}
						VNS(M1_sch_buffer,M2_sch_buffer,M3_sch_buffer);
						bilanciamento_schedule(M1_sch_buffer,M2_sch_buffer,M3_sch_buffer);//bilancio 
						valuta_schedula(M1_sch_buffer,M2_sch_buffer,M3_sch_buffer,prossimo1);
			
						elimina_schedula(M1_sch_buffer);
						if(num_macchine>=2)
						{
							elimina_schedula(M2_sch_buffer);
						}
						if(num_macchine==3)
						{
		
							elimina_schedula(M3_sch_buffer);
						}
						next_elem *tmp_prox;
						tmp_prox=prossimo;
						while(tmp_prox->next!=NULL)
						{
							tmp_prox = tmp_prox->next;
						}//trovo l'ultimo elemento.
						if 
						(
							(prossimo1->feasible>tmp_prox->feasible)
							||
							(
								(prossimo1->feasible==tmp_prox->feasible)
								&&
								(prossimo1->Lmax<tmp_prox->Lmax)
							)
							||
							(	
								(prossimo1->feasible==tmp_prox->feasible)
								&&
								(prossimo1->Lmax==tmp_prox->Lmax)
								&&
								(prossimo1->Cmax<tmp_prox->Cmax)
							)
							||
							(	
								(prossimo1->feasible==tmp_prox->feasible)
								&&
								(prossimo1->Lmax==tmp_prox->Lmax)
								&&
								(prossimo1->Cmax==tmp_prox->Cmax)
								&&
								(prossimo1->Tardy<tmp_prox->Cmax)
		
							)
						)				
						{
							tmp_prox->Lmax=prossimo1->Lmax;
							tmp_prox->Cmax=prossimo1->Cmax;
							tmp_prox->Tardy=prossimo1->Tardy;
							tmp_prox->feasible=prossimo1->feasible;	
						}
						
						free(prossimo1);
					}							
				}
				//se la schedula non �feasible deve essere penalizzata rispetto alle altre.
				//devo ridurre il numero di job che rimangono da schedulare
				//devo trovare il job con la Lateness + alta 
		// 		in condizioni di parit�quello con la Cmax +bassa
		// 		infine con il numero + basso di Tardy job
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
					}
					temp=temp->next;		
				}//ora sappiamo qual �L'ID del Job da eliminare dalla lista dei job da schedulare
		// 		e su quale macchina �stato schedulato	
 				printf("\n (%i) %i %i %i %i\n",cont_livelli, ID_heur, L_max, C_max,tardy);				
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
				
// 				if(prossimo!=NULL)
// 				{
				next_elem *temp_pr;
				for(i=0;i<num_heur;i++)
				{	
					temp_pr = prossimo->next;
					free(prossimo);
					prossimo = temp_pr;
				}
				
			/*	}*/		
						/*elimino i vari candidati di questo step e procedo allo step successivo*/
				for(i=0;i<num_heur;i++)
				{
					free(permutazioni[i]);
				}
//  			array_job_attuale[iter_for].ID=iter_for;
			}
// 		verifica_cambiamento_macchina(&primo_passo_M1,&primo_passo_M2,&primo_passo_M3);				
		}
 		printf("\n %i____________________\n",cont_livelli);				
//_____________________________________________________________________________________________________________________				
		Pos_vincitore=seleziona_prossimo_job(lista_prossimi_vincitori,cont_livelli);//da fare
		//Pos_vincitore indica la posizione relativa del job da schedulare
		Pos_assoluta=trova_posizione_assoluta(array_job_attuale,Pos_vincitore);
// 		printf("\nla posizione_vincitore �: %i Pos_assoluta �: %i",Pos_vincitore,Pos_assoluta);
// 		printf("\n l'id del job successore a quello da inserire �:%i \n",lista_prossimi_vincitori[Pos_vincitore].ID_job);
		// devo confrontare la migliore permutazione con quelle generate in questo passo dal rollout
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
	// 		printf(" %i\n",best_perm[num_job-cont_livelli-1].ID);
			best_perm[num_job-cont_livelli-1].tipo=array_job_attuale[Pos_assoluta].tipo;
			best_perm[num_job-cont_livelli-1].proc_time=array_job_attuale[Pos_assoluta].proc_time;
			best_perm[num_job-cont_livelli-1].duedate=array_job_attuale[Pos_assoluta].duedate;
			best_perm[num_job-cont_livelli-1].deadline=array_job_attuale[Pos_assoluta].deadline;
			best_perm[num_job-cont_livelli-1].priority=array_job_attuale[Pos_assoluta].priority;
			best_perm[num_job-cont_livelli-1].rel_time=array_job_attuale[Pos_assoluta].rel_time;
			array_job_attuale[Pos_assoluta].ID=-1;		
//          fprintf(f_log,"%i %i %i %i \n",lista_prossimi_vincitori[Pos_vincitore].feasible,lista_prossimi_vincitori[Pos_vincitore].Lmax,lista_prossimi_vincitori[Pos_vincitore].Cmax,lista_prossimi_vincitori[Pos_vincitore].Tardy);
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
// 			printf("\a\nhelp!\n");

//         fprintf(f_log,"%i %i %i %i \n",Feasible_best,Lmax_best,Cmax_best,Tardy_best);
		} 
// 		printf("\n P_vincitore %i job aggiunto= %i \n",Pos_vincitore,best_perm[num_job-cont_livelli-1].ID);	
// 		free(lista_prossimi_vincitori);
// 		printf("\a\n___!\n");
// 		stampa_permutazioni( permutazione_buffer,num_job);
// 		printf("\a\n___!\n");
		cont_livelli--;      
	}
//    fclose(f_log);
// 	stampa_risultati_a_video(0);
	i=0;
	while(array_job_attuale[i].ID==-1)
	{
		i++;
	}
	best_perm[num_job-1].ID=array_job_attuale[i].ID;
// 	printf(" %i\n",best_perm[num_job-1].ID);
	best_perm[num_job-1].tipo=array_job_attuale[i].tipo;
	best_perm[num_job-1].proc_time=array_job_attuale[i].proc_time;
	best_perm[num_job-1].duedate=array_job_attuale[i].duedate;
	best_perm[num_job-1].deadline=array_job_attuale[i].deadline;
	best_perm[num_job-1].priority=array_job_attuale[i].priority;
	best_perm[num_job-1].rel_time=array_job_attuale[i].rel_time;
	array_job_attuale[i].ID=-1;
	free(job_fisso);
	free(array_job_attuale);
	azzera_schedule();
	next_elem *prossimo1;
 	prossimo1= malloc(sizeof(next_elem));
	prossimo1->next=NULL; 
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
// 		printf("\n Uso La permutazione_buffer\n\a");
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




