/***************************************************************************
 *   Copyright (C) 2006 by Michele Ciavotta   *
 *   ciavotta@dia.uniroma3.it   *
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

job *permutazione_base(job *array_job_attuale, int dim_job)
{//questa e' l'euristica di base presentata nella tesi di Ritota
	job1 *array_job_locale;		
	int i= 0;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;	
	schedula **schedule_locali;
	job * perm1;
	int jj;
	int k = 0;

	array_job_locale=malloc(dim_job*sizeof(job1));
//	inizializzo le schedule locali che serviranno per la individuazione della sequenza di job
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	
// 	schedule_locali a' un vettore di puntatori alle schedule locali
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	//inizializzo le schedule locali
	copia_schelule(M1_sch,M1_sch_locale);// fa una copia di M1_sch in M1_sch_locale	
	schedule_locali[0] = M1_sch_locale;
	if(num_macchine >= 2)
	{
		copia_schelule(M2_sch,M2_sch_locale);
		schedule_locali[1] = M2_sch_locale;
	}
	if(num_macchine == 3)
	{
		copia_schelule(M3_sch,M3_sch_locale);
		schedule_locali[2] = M3_sch_locale;
	}
// ________________________________________________________________________________
// copio le informazioni contenute in array_job_attuale 	
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
//__________________________________________________________________________________
// 	Qui comincia la parte principale dell'algoritmo
	i=0;
	while (i<dim_job)// fin quando non ho schedulato tutti i job
	{
		temp = aggiungi_job_perm(array_job_locale,dim_job,schedule_locali);

		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
				break;
			}
		k++;
		}
		for(jj=0;jj<dim_job;jj++)
		{
			array_job_locale[jj].adatto = 0;// devo verificare se e' cambiato
		}
	i++;	
	}
// ____________________________________________________________________________________
	
//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
tipo_new carica_tempo(schedula *M_sch, elem *M)
{	
	tipo_new temp;
	int fine = 0;	
	while(M_sch->next != NULL)
	{
		M_sch = M_sch->next;	
	}
	fine = M_sch->fine;
	while
	(	
		(M != NULL)
		&&
		(fine > M->inizio)
	)
	{
		M = M->next;
	}
	if
	(
		(M == NULL)
		||
		(fine < M->inizio)
	)
	{
		temp.fine = fine;
		temp.tipo = M_sch->tipo;
		temp.Camp = M_sch->index_camp;
		return temp;
	}
	else
	{
		temp.fine = M->fine;
		temp.tipo = M_sch->tipo;
		temp.Camp = M_sch->index_camp;
		return temp;
	}//assumo che nn possano esserci due interruzioni consecutive senza un periodo di attivit��?nel mezzo
	
}

job *permutazione_delta_10(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;	
	int i= 0;
	int Delta = 10;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;

	array_job_locale=malloc(dim_job*sizeof(job1));	
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);	
	return perm1;

}
job *permutazione_delta_15(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;	
	int i= 0;
	int Delta = 15;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;		

	array_job_locale=malloc(dim_job*sizeof(job1));	
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_20(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;		
	int i= 0;
	int Delta = 20;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;
	
	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_25(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;		
	int i= 0;
	int Delta = 25;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;
	
	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_30(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;	
	int i= 0;
	int Delta = 30;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;
	
	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_35(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;		
	int i= 0;
	int Delta = 35;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;

	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_40(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;	
	int i= 0;
	int Delta = 40;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;

	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_50(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;		
	int i= 0;
	int Delta = 50;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;

	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}

job *permutazione_delta_24ore(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    job1 *array_job_locale;     
    int i= 0;
    int Delta = 840;
    int temp = 0;
    schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;   
    schedula **schedule_locali;
    job * perm1;
    int k;

    array_job_locale=malloc(dim_job*sizeof(job1));	
    M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
    if(num_macchine >= 2)
    {
        M2_sch_locale = malloc(sizeof(schedula));
    }
    if(num_macchine == 3)
    {
        M3_sch_locale = malloc(sizeof(schedula));
    }
    schedule_locali=malloc(num_macchine*sizeof(schedula *));
    perm1 = malloc(dim_job*sizeof(job));
    {//inizializzo le schedule locali
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine >= 2)
        {
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    elimina_schedula(M1_sch_locale);
    if(num_macchine >= 2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    free(schedule_locali);
    free(array_job_locale);
    return perm1;

}
job *permutazione_delta_7ore(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    job1 *array_job_locale; 
    int i= 0;
    int Delta = 420;
    int temp = 0;
    schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;   
    schedula **schedule_locali;
    job * perm1;
    int k;

    array_job_locale=malloc(dim_job*sizeof(job1));		
    M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
    if(num_macchine >= 2)
    {
        M2_sch_locale = malloc(sizeof(schedula));
    }
    if(num_macchine == 3)
    {
        M3_sch_locale = malloc(sizeof(schedula));
    }
    schedule_locali=malloc(num_macchine*sizeof(schedula *));
    perm1 = malloc(dim_job*sizeof(job));
    {//inizializzo le schedule locali
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine >= 2)
        {
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
 	k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    elimina_schedula(M1_sch_locale);
    if(num_macchine >= 2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    free(schedule_locali);
    free(array_job_locale);
    return perm1;

}
job *permutazione_fittizia(job *array_job_attuale, int dim_job)
{//questa euristica costruisce la permutazione in base al proc_time del job
	job *perm1;
	perm1 = malloc(dim_job*sizeof(job));
	int i;
	if(dim_job==num_job)
	{		
		for(i=0;i<dim_job;i++)
		{
			perm1[i]=array_job_attuale[i];
			best_perm[i]=array_job_attuale[i];
		}
		
	}
	else
	{//copio una parte della migliore perturbazione trovata
		for(i=0;i<dim_job;i++)
		{
			perm1[i]=best_perm[num_job-dim_job+i];		
		}
	}
	return perm1;
}
job *permutazione_delta_3(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;	
	int i= 0;
	int Delta = 3;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;	
	schedula **schedule_locali;
	job * perm1;
	int k;

	array_job_locale=malloc(dim_job*sizeof(job1));	
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_5(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;	
	int i= 0;
	int Delta = 5;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;

	array_job_locale=malloc(dim_job*sizeof(job1));	
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
job *permutazione_delta_2(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
	job1 *array_job_locale;		
	int i= 0;
	int Delta = 2;
	int temp = 0;
	schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
	schedula **schedule_locali;
	job * perm1;
	int k;
	
	array_job_locale=malloc(dim_job*sizeof(job1));
	M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
	if(num_macchine >= 2)
	{
		M2_sch_locale = malloc(sizeof(schedula));
	}
	if(num_macchine == 3)
	{
		M3_sch_locale = malloc(sizeof(schedula));
	}
	schedule_locali=malloc(num_macchine*sizeof(schedula *));
	perm1 = malloc(dim_job*sizeof(job));
	{//inizializzo le schedule locali
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	i=0;
	while (i<dim_job)
	{
		temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
		k = 0;
		while(k<dim_job)
		{
			if(temp == array_job_locale[k].ID)
			{
				perm1[i].ID =	array_job_locale[k].ID;
				perm1[i].tipo =	array_job_locale[k].tipo;
				perm1[i].proc_time = array_job_locale[k].proc_time;
				perm1[i].duedate = array_job_locale[k].duedate;
				perm1[i].deadline = array_job_locale[k].deadline;
				perm1[i].priority = array_job_locale[k].priority;
				perm1[i].rel_time = array_job_locale[k].rel_time;
				array_job_locale[k].schedulato = 1;
			}
		k++;
		}
		array_job_locale[i].adatto = 0;
	i++;	
	}
	//devo ora liberare lo spazio delle schedule locali
	elimina_schedula(M1_sch_locale);
	if(num_macchine >= 2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	free(schedule_locali);
	free(array_job_locale);
	return perm1;

}
int aggiungi_job_perm_delta (job1 *array_job_locale,int dim_job,schedula **schedule_locali,int delta)
{
	job *perm;
	elem **indisp;
	int *disponibilita;
	int disp = 0;
	tipo_new *tempo;
	int *deadline_vett;	
	int i,k,minimum,tipo,Camp;
	int *duedate_vett;
	int *else_vett;
	int deadline = -1;
	int duedate = -1;
	int nothing = -1;
        int minimum_alternativo2;
	int *set_duedate;//insieme dei job possibilmente schedulabili (minimi nella duedate o deadline)	
	int *set_deadline;
	int *set_nothing;
	int *tipo_job_duedate;
	int *tipo_job_deadline;
	int *tipo_job_nothing;
	int *tipo_macchine;
	int *campagna_macchine;
	int **setup_matrix;
	int **set_matrix;
        int minimum_alternativo=100000;
        int tempi_ind[3];
	int fine;
	int min_deadline;
	int min_duedate;
	int cambio;
	int jj;
	int *setup_vett;
	int st_vt;
	int kk;
	int j1;
	int pos_i = 0;
	int pos_k = 0;

	perm = malloc(sizeof(job)); 
	indisp=malloc(num_macchine*sizeof(elem *));
	disponibilita= &disp;
	tempo=malloc(num_macchine*sizeof(tipo_new));
	set_duedate=malloc(dim_job*sizeof(int));
	set_deadline=malloc(dim_job*sizeof(int));
	set_nothing=malloc(dim_job*sizeof(int));
	tipo_job_duedate=malloc(dim_job*sizeof(int));
	tipo_job_deadline=malloc(dim_job*sizeof(int));
	tipo_job_nothing=malloc(dim_job*sizeof(int));
	tipo_macchine=malloc(num_macchine*sizeof(int));
	campagna_macchine=malloc(num_macchine*sizeof(int));
	deadline_vett = malloc(dim_job*sizeof(int)); 
	duedate_vett = malloc(dim_job*sizeof(int)); 
	else_vett= malloc(dim_job*sizeof(int));
	setup_matrix = malloc(dim_job*sizeof(int *));	
	set_matrix = malloc(dim_job*sizeof(int *));
	for(i=0;i<dim_job;i++)
	{
		setup_matrix[i] = malloc(num_macchine*sizeof(int));
		set_matrix[i] = malloc(num_macchine*sizeof(int));
	}
	indisp[0] = M1;
	if(num_macchine >= 2)
	{
	indisp[1] = M2;
	}
	if(num_macchine == 3)
	{
		indisp[2] = M3;
	}
	for(i=0; i<dim_job; i++)
	{
		for(k=0; k<num_macchine; k++)
		{
			setup_matrix[i][k]=-1;
			set_matrix[i][k]=-1;
			if(i == 0)
			{
				tipo_macchine[k] = -1;
				campagna_macchine[k] = -1;
			}
		}
		deadline_vett[i] = -1;
		duedate_vett[i] = -1;
		else_vett[i] = -1;
		set_duedate[i] = -1;
		set_deadline[i]= -1;
		set_nothing[i] = -1;
		tipo_job_duedate[i] = -1;	
		tipo_job_deadline[i] = -1;	
		tipo_job_nothing[i] = -1;
	}
	tempo[0] = carica_tempo(schedule_locali[0],M1);//carica gli starting time disponibili
	if(num_macchine >= 2)
	{
		tempo[1] = carica_tempo(schedule_locali[1],M2);
	}
	if (num_macchine == 3)
	{
		tempo[2] = carica_tempo(schedule_locali[2],M3);
	}
// 	ora conosco i tempi minimi di schedulazione sulle singole macchine
// 	a questo punto devo solo cercare il minimo di questi tempi e dato questo stabilire il set di job con release dete
// 	anteriore a tale data.
	minimum = tempo[0].fine;
	for (i = 1; i < num_macchine; i++)
	{
		minimum = min(minimum,tempo[i].fine);
	}//ora conosco il minimo 
// 	devo costruire il set dei job released
	for(i=0; i<dim_job; i++)
	{
		if
		(
			(array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
			&&
			(array_job_locale[i].rel_time <= minimum)
		)
		{
			array_job_locale[i].adatto = 1;
			if(array_job_locale[i].deadline > 0)
			{
				deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline
			}
			if(array_job_locale[i].duedate > 0)//qui ho modificato pu��?esserci lo stesso job su + vettori
			{
				duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate
			}
			if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
			{
				else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline
			}		
		}
                if
                (
                 (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                 &&
                 (array_job_locale[i].rel_time <= minimum_alternativo)
                )
                {
                   minimum_alternativo=array_job_locale[i].rel_time;
                }
	}//ora ho segnato i job che posso schedulare xch��?rispettano la release date.
// 	cerco ora un elemento con deadline != 0;
        fine=0;
        k=-1;
	for(i=0;i<dim_job;i++)
	{
		if(deadline_vett[i] > 0)
		{
			k=i;
			break;	
		}
	}
	min_deadline = 0;
	if(k>=0)
	{
	 min_deadline = deadline_vett[k];
// 	ho trovato almeno un elemento che ha deadline > 0
         for(i=k+1;i<dim_job;i++)
         {
               if
               (
                        (deadline_vett[i] != -1)
                        &&
                        (deadline_vett[i] < deadline_vett[k])
               )
               {
                        k=i;// salvo la posizione
                        min_deadline = deadline_vett[k];
               }
         }
        fine=1;
	deadline = 1;// segno che ho trovato una deadline	
	}
	min_duedate=0;
	k=-1;
	for(i=0;i<dim_job;i++)
	{
		if(duedate_vett[i] > 0)
		{
			k=i;
			break;	
		}
	}
	if(k>=0)
	{
		min_duedate = duedate_vett[k];
// 	ho trovato almeno un elemento che ha duedate > 0 
		for(i=k+1;i<dim_job;i++)
		{
			if
			(
				(duedate_vett[i] != -1)
				&&
				(duedate_vett[i] < duedate_vett[k])
				)
			{
				k=i;// salvo la posizione
				min_duedate = duedate_vett[k];
			}
		}
                fine=1;
                duedate = 1;
	}
        else //cerco un job privo di duedate e deadline
           {		
              k=-1;
              for(i=0;i<dim_job;i++)
              {
                 if(else_vett[i] > 0)
                 {
                    k=i;
                    break;	
                 }
              }
              if(k>=0)
              {
                 fine = 1; 
                 nothing = 1;
              }
			
           }
        if(fine==0)
        {
           for(i=0;i<num_macchine;i++)
           {
              tempi_ind[i]=trova_edge_indisp(indisp[i],minimum_alternativo);
           }
      
           minimum_alternativo2=tempi_ind[0];
           for (i = 1; i < num_macchine; i++)
           {
              if(tempi_ind[i]<minimum_alternativo2)
              {
                 minimum_alternativo2=tempi_ind[i];
              }
			
           }
           if (minimum_alternativo>minimum_alternativo2)
           {
              i=0;
              for(i=0;i<num_macchine;i++)
              {
                 if(tempi_ind[i]<minimum_alternativo)
                 {
                    tempi_ind[i]=minimum_alternativo;
                 }
                 tempo[i].fine=tempi_ind[i];
              }
              minimum=minimum_alternativo;
           }
           else
           {
              i=0;
              for(i=0;i<num_macchine;i++)
              {
                 tempo[i].fine=tempi_ind[i];
              }
              minimum=minimum_alternativo2;
           }
           minimum_alternativo=max(minimum_alternativo,minimum_alternativo2);
           for(i=0; i<dim_job; i++)
           {
              if
              (
               (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
               &&
               (array_job_locale[i].rel_time <= minimum)
              )
              {
                 array_job_locale[i].adatto = 1;
                 if(array_job_locale[i].deadline > 0)
                 {
                    deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline
                 }
                 if(array_job_locale[i].duedate > 0)//qui ho modificato pu��?esserci lo stesso job su + vettori
                 {
                    duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate
                 }
                 if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
                 {
                    else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline
                 }		
              }
           }
           k=-1;
           for(i=0;i<dim_job;i++)
           {
              if(deadline_vett[i] > 0)
              {
                 k=i;
                 break;	
              }
           }
           min_deadline = 0;
           if(k>=0)
           {
              min_deadline = deadline_vett[k];
// 	ho trovato almeno un elemento che ha deadline > 0
              for(i=k+1;i<dim_job;i++)
              {
                 if
                 (
                  (deadline_vett[i] != -1)
                  &&
                  (deadline_vett[i] < deadline_vett[k])
                 )
                 {
                    k=i;// salvo la posizione
                    min_deadline = deadline_vett[k];
                 }
              }
              fine=1;
              deadline = 1;// segno che ho trovato una deadline	
           }
           min_duedate=0;
           k=-1;
           for(i=0;i<dim_job;i++)
           {
              if(duedate_vett[i] > 0)
              {
                 k=i;
                 break;	
              }
           }
           if(k>=0)
           {
              min_duedate = duedate_vett[k];
// 	ho trovato almeno un elemento che ha duedate > 0 
              for(i=k+1;i<dim_job;i++)
              {
                 if
                 (
                  (duedate_vett[i] != -1)
                  &&
                  (duedate_vett[i] < duedate_vett[k])
                 )
                 {
                    k=i;// salvo la posizione
                    min_duedate = duedate_vett[k];
                 }
              }
              fine=1;
              duedate = 1;
           }
            //cerco un job privo di duedate e deadline
           		
            k=-1;
            for(i=0;i<dim_job;i++)
            {
               if(else_vett[i] > 0)
               {
                  k=i;
                  break;	
               }
            }
            if(k>=0)
            {
               fine = 1; 
               nothing = 1;
            }
           
           /*if(duedate != 1 && deadline != 1) //cerco un job privo di duedate e deadline
           {		
              k=-1;
              for(i=0;i<dim_job;i++)
              {
                 if(else_vett[i] > 0)
                 {
                    k=i;
                    break;	
                 }
              }
              if(k>=0)
              {
                 fine = 1;
                 nothing = 1; 
              }	
           }  */         
        }
	//calcolo ora il minimo tra duedate e deadline se min resta uguale a zero significa che devo considerare gli elementi del vettore else
	int minimus =0;
	if(deadline == -1 && duedate==1)
	{
		minimus = min_duedate;
	}
	else if(deadline == 1 && duedate== -1)
	{
		minimus = min_deadline;
	}
	else if (deadline == 1 && duedate==1)
	{
		minimus = min(min_deadline,min_duedate);
	}
		
	if (deadline == 1)
	{//minimus indica il valore + basso della deadline
		i=0;
		cambio = 0;
		while(i<dim_job)
		{
			if
			(
				(deadline_vett[i]<=minimus+delta)//scelgo i job a distanza delta dal minimo
				&&
				(deadline_vett[i]>0)
			)
			{
				set_deadline[i] = array_job_locale[i].ID;
				tipo_job_deadline[i] = array_job_locale[i].tipo;
				cambio = 1;
			}
			i++;
		}
		if(cambio == 0)
		{//se nn ho trovato una deadline in delta
			deadline = -1;
		}
		else
		{
			duedate = -1;//da questo punto in poi do precedenza alle deadline
			nothing = -1;
		}
		
	}
	if (duedate == 1)
	{
		//minimus indica il valore + basso della deadline
		i=0;
		cambio = 0;
		while(i<dim_job)
		{
			if
			(
				(duedate_vett[i]<=minimus+delta)
				&&
				(duedate_vett[i]>0)
			)
			{
				set_duedate[i] = array_job_locale[i].ID;
				tipo_job_duedate[i] = array_job_locale[i].tipo;
				cambio = 1;
			}
			i++;
		}
		if(cambio == 0)
		{//se nn ho trovato una duedate in delta
			duedate = -1;
		}
		else
		{
                  deadline = -1;
		  nothing = -1;
		}
	} 
	if (nothing == 1)
	{
		//minimus indica il valore + basso della deadline
		i=0;
		while(i<dim_job)
		{
			if(else_vett[i]!=-1)
			{
				set_nothing[i] = array_job_locale[i].ID;
				tipo_job_nothing[i] = array_job_locale[i].tipo;
				
			}
			i++;
		}
	}
        
	i=0;
	while(i<num_macchine)
	{
		if(tempo[i].fine == minimum)
		{
			tipo_macchine[i] = tempo[i].tipo;
			campagna_macchine[i] = tempo[i].Camp;
		}
		i++;
	}
        
	if(deadline ==1) //privilegio sempre le deadline se ci sono
	{
		for(i=0;i<dim_job;i++)
		{	
			for(k=0;k<num_macchine;k++)
			{
				if
				(
					(set_deadline[i]!=-1)
					&&
					(tipo_macchine[k]!=-1)
				)
				{
					if(tipo_job_deadline[i] == tipo_macchine[k])
					{
						int j = 0;
						for(j=0;j<num_tipi;j++)
						{
							if(array_tipi[j].ID==tipo_macchine[k])
							{
								if(campagna_macchine[k]<array_tipi[j].MaxOpCamp)
								{
// 					eureka posso shedulare questo job immediatamente
// 					la macchina k-esima verra' assegnata al job
								jj=0;					
								st_vt = 0;
								setup_vett =&st_vt;
								while(jj<num_job)
								{
									if(array_job[jj].ID == set_deadline[i])
									{
										perm[0]=array_job[jj];
										break;
									}
								jj++;
								}
								verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
								aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
								kk = set_deadline[i];
								free(deadline_vett); 
								free(duedate_vett); 
								free(else_vett);
								for(i=0;i<dim_job;i++)
								{
									free(setup_matrix[i]);
									free(set_matrix[i]);
								}
								free(setup_matrix);
								free(set_matrix);
								free(tempo);
								free(indisp);
								free(set_duedate);
								free(set_deadline);
								free(set_nothing);
								free(tipo_job_duedate);
								free(tipo_job_deadline);
								free(tipo_job_nothing);
								free(tipo_macchine);
								free(campagna_macchine);
								free(perm);	
								return kk;// siccome aggiungo un job alla volta nn serve che io cancelli l'elemento corrispondente nel vettore delle duedate
								}
								else
								{
								setup_matrix[i][k]= 	Cmaj_matrix[campagna_macchine[k]-1][tipo_job_deadline[i]-1];
								set_matrix[i][k] = set_deadline[i];
								}	
							}
						}	
					}
					else
					{
						if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gia' schedulati
						{
// 						nn devo pagare setup
							jj=0;
							st_vt = 0;
							setup_vett =&st_vt;
							while(jj<num_job)
							{
								if(array_job[jj].ID == set_deadline[i])
								{
									perm[0]=array_job[jj];
									break;
								}
							jj++;
							}
							verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
							aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
							kk = set_deadline[i];
							free(deadline_vett); 
							free(duedate_vett); 
							free(else_vett);
							for(i=0;i<dim_job;i++)
							{
								free(setup_matrix[i]);
								free(set_matrix[i]);
							}
							free(setup_matrix);
							free(set_matrix);
							free(tempo);
							free(indisp);
							free(set_duedate);
							free(set_deadline);
							free(set_nothing);
							free(tipo_job_duedate);
							free(tipo_job_deadline);
							free(tipo_job_nothing);
							free(tipo_macchine);
							free(campagna_macchine);
							free(perm);	
							return kk;							
						}
						setup_matrix[i][k]= Cmaj_matrix[campagna_macchine[k]-1][tipo_job_deadline[i]-1];
						set_matrix[i][k] = set_deadline[i];
					}
				}
			}
		}
	}
	else if(duedate == 1) //non ci sono deadline e uso le duedate
	{
		for(i=0;i<dim_job;i++)
		{	
			for(k=0;k<num_macchine;k++)
			{
				if
				(
					(set_duedate[i]!=-1)
					&&
					(tipo_macchine[k]!=-1)
				)
				{
					if(tipo_job_duedate[i] == tipo_macchine[k])
					{
						jj = 0;
						for(jj=0;jj<num_tipi;jj++)
						{
							if(array_tipi[jj].ID==tipo_macchine[k])
							{
								if(campagna_macchine[k]<array_tipi[jj].MaxOpCamp)
								{
// 					eureka posso shedulare questo job immediatamente
// 					la macchina k-esima verr��?assegnata al job
								j1=0;
								st_vt = 0;
								setup_vett =&st_vt;
								while(j1<num_job)
								{
									if(array_job[j1].ID == set_duedate[i])
									{
										perm[0]=array_job[j1];
										break;
									}
								j1++;
								}
								verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
								aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
								kk = set_duedate[i];
								free(deadline_vett); 
								free(duedate_vett); 
								free(else_vett);
								for(i=0;i<dim_job;i++)
								{
									free(setup_matrix[i]);
									free(set_matrix[i]);
								}
								free(setup_matrix);
								free(set_matrix);
								free(tempo);
								free(indisp);
								free(set_duedate);
								free(set_deadline);
								free(set_nothing);
								free(tipo_job_duedate);
								free(tipo_job_deadline);
								free(tipo_job_nothing);
								free(tipo_macchine);
								free(campagna_macchine);
								free(perm);	
								return kk;	
								// siccome aggiungo un job alla volta nn serve che io cancelli l'elemento corrispondente nel vettore delle duedate
								}
								else
								{
								setup_matrix[i][k]= 	Cmaj_matrix[campagna_macchine[k]-1][tipo_job_duedate[i]-1];
								set_matrix[i][k] = set_duedate[i];
								}	
							}
						}	
					}
					else
					{
						if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gi��?schedulati
						{
// 						nn devo pagare setup
							//MIA
							jj=0;
							st_vt = 0;
							setup_vett =&st_vt;
							while(jj<num_job)
							{
								if(array_job[jj].ID == set_duedate[i])
								{
									perm[0]=array_job[jj];
									break;
								}
							jj++;
							}
							verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
							aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
							kk = set_duedate[i];
							free(deadline_vett); 
							free(duedate_vett); 
							free(else_vett);
							for(i=0;i<dim_job;i++)
							{
								free(setup_matrix[i]);
								free(set_matrix[i]);
							}
							free(setup_matrix);
							free(set_matrix);
							free(tempo);
							free(indisp);
							free(set_duedate);
							free(set_deadline);
							free(set_nothing);
							free(tipo_job_duedate);
							free(tipo_job_deadline);
							free(tipo_job_nothing);
							free(tipo_macchine);
							free(campagna_macchine);
							free(perm);	
							return kk;
							}
						setup_matrix[i][k]= Cmaj_matrix[campagna_macchine[k]-1][tipo_job_duedate[i]-1];
						set_matrix[i][k] = set_duedate[i];
					}
				}
			}
		}
	}
	else if (nothing == 1)
	{
		for(i=0;i<dim_job;i++)
		{	
			for(k=0;k<num_macchine;k++)
			{
				if
				(
					(set_nothing[i]!=-1)
					&&
					(tipo_macchine[k]!=-1)
				)
				{
					if(tipo_job_nothing[i] == tipo_macchine[k])
					{
						jj = 0;
						for(jj=0;jj<num_tipi;jj++)
						{
							if(array_tipi[jj].ID==tipo_macchine[k])
							{
								if(campagna_macchine[k]<array_tipi[jj].MaxOpCamp)
								{
	// 								eureka posso shedulare questo job immediatamente
	// 								la macchina k-esima verr��?assegnata al job
									j1=0;	
									st_vt = 0;
									setup_vett =&st_vt;
									while(j1<num_job)
									{
										if(array_job[j1].ID == set_nothing[i])
										{
											perm[0]=array_job[j1];
											break;
										}
									j1++;
									}
									verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
									aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
									kk = set_nothing[i];
									free(deadline_vett); 
									free(duedate_vett); 
									free(else_vett);
									for(i=0;i<dim_job;i++)
									{
										free(setup_matrix[i]);
										free(set_matrix[i]);
									}
									free(setup_matrix);
									free(set_matrix);
									free(tempo);
									free(indisp);
									free(set_duedate);
									free(set_deadline);
									free(set_nothing);
									free(tipo_job_duedate);
									free(tipo_job_deadline);
									free(tipo_job_nothing);
									free(tipo_macchine);
									free(campagna_macchine);
									free(perm);	
									return kk;	
									// siccome aggiungo un job alla volta nn serve che io cancelli l'elemento corrispondente nel vettore delle nothing
								}
								else
								{
								setup_matrix[i][k]= 	Cmaj_matrix[campagna_macchine[k]-1][tipo_job_nothing[i]-1];
								set_matrix[i][k] = set_nothing[i];
								}	
							}
						}	
					}
					else
					{
						if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gi��?schedulati
						{
// 						nn devo pagare setup
							jj=0;							
							st_vt = 0;
							setup_vett =&st_vt;
							while(jj<num_job)
							{
								if(array_job[jj].ID == set_nothing[i])
								{
									perm[0]=array_job[jj];
									break;
								}
							jj++;
							}
							verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
							aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
							kk = set_nothing[i];
							free(deadline_vett); 
							free(duedate_vett); 
							free(else_vett);
							for(i=0;i<dim_job;i++)
							{
								free(setup_matrix[i]);
								free(set_matrix[i]);
							}
							free(setup_matrix);
							free(set_matrix);
							free(tempo);
							free(indisp);
							free(set_duedate);
							free(set_deadline);
							free(set_nothing);
							free(tipo_job_duedate);
							free(tipo_job_deadline);
							free(tipo_job_nothing);
							free(tipo_macchine);
							free(campagna_macchine);
							free(perm);	
							return kk;							
						}
						setup_matrix[i][k]= Cmaj_matrix[campagna_macchine[k]-1][tipo_job_nothing[i]-1];
						set_matrix[i][k] = set_nothing[i];
					}
				}
			}
		}
	}	
			
// 	 a questo punto ho la matrice dei setup e devo scegliere quello a costo minimo.
	pos_i = 0;
	pos_k = 0;
	for(i=0;i<dim_job;i++)
	{
		for(k=0;k<num_macchine;k++)
		{
// 			devo trovare un elemento nn nullo
			if
			(
				(setup_matrix[pos_i][pos_k] == -1)
				&&
				(setup_matrix[i][k]!=-1)
			)
			{
				pos_i = i;
				pos_k = k;
			}
			if
			(
				(setup_matrix[i][k]>=0)
				&&
				(setup_matrix[i][k] < setup_matrix[pos_i][pos_k])
			)
			{
				pos_i = i;
				pos_k = k;
			}
		}
	}
	jj=0;
	st_vt = 1;
	setup_vett =&st_vt;
	while(jj<num_job)
	{
		if(array_job[jj].ID == set_matrix[pos_i][pos_k])
		{
			perm[0]=array_job[jj];
			break;
		}
	jj++;
	}
	verifica_macchina(schedule_locali[pos_k],indisp[pos_k],disponibilita,setup_vett,0,perm,0);
	aggiungi_schedula(schedule_locali[pos_k],perm[0],disponibilita[0],setup_vett[0]); 
	kk = set_matrix[pos_i][pos_k];
	free(deadline_vett); 
	free(duedate_vett); 
	free(else_vett);
	for(i=0;i<dim_job;i++)
	{
		free(setup_matrix[i]);
		free(set_matrix[i]);
	}
	free(setup_matrix);
	free(set_matrix);
	free(tempo);
	free(indisp);
	free(set_duedate);
	free(set_deadline);
	free(set_nothing);
	free(tipo_job_duedate);
	free(tipo_job_deadline);
	free(tipo_job_nothing);
	free(tipo_macchine);
	free(campagna_macchine);
	free(perm);
	return kk;
}

// euristica LLF largest Lateness First
job *permutazione_LLF(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
	elem **vett_indisp;
	vett_indisp=malloc(num_macchine*sizeof(elem *));
	int **vett_Lat;//contiene il valore della lateness per ogni macchina
	int **vett_End;//contiene il valore dell'ending time per ogni macchina
	int **vett_setup;
	
	int i,j,done_tot = 0,k = 0;
	job *perm;
	perm = malloc(sizeof(job));
	int **disponibilita;
	int **scelti;
	int *vett_max_Lat;
	vett_max_Lat = malloc(dim_job*sizeof(int));
	disponibilita = malloc(dim_job*sizeof(int *));
	vett_Lat = malloc(dim_job*sizeof(int *));
	vett_End = malloc(dim_job*sizeof(int *));
	vett_setup = malloc(dim_job*sizeof(int *));
	scelti =malloc(dim_job*sizeof(int *));	
	for(i=0;i<dim_job;i++)
	{
		disponibilita[i]=malloc(num_macchine*sizeof(int));
		vett_Lat[i]=malloc(num_macchine*sizeof(int));
		vett_End[i]=malloc(num_macchine*sizeof(int));
		vett_setup[i]=malloc(num_macchine*sizeof(int));
		scelti[i]=malloc(num_macchine*sizeof(int));
	}
	schedula *M1_sch_locale;
	schedula *M2_sch_locale;
	schedula *M3_sch_locale;
	schedula **schedule_locali;
	schedule_locali=malloc(num_macchine*sizeof(schedula*));
	job * perm1;
	job1 *array_job_locale;	
	
	perm1 = malloc(dim_job*sizeof(job));
	array_job_locale=malloc(dim_job*sizeof(job1));
	

	{//inizializzo le schedule locali
		M1_sch_locale = malloc(sizeof(schedula));
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine >= 2)
		{
			M2_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			M3_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	if(num_macchine == 1)
	{
		vett_indisp[0] = M1;
	}
	if(num_macchine == 2)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
	}
	if(num_macchine == 3)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
		vett_indisp[2] = M3;
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	int kk=0;
	while(done_tot==0)
	{
		int *setup_vett;
		int st_vt;
		setup_vett=&st_vt;
		for(i = 0; i < dim_job; i++)
		{
			if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?		
			{
				perm[0].ID= array_job_attuale[i].ID;
				perm[0].tipo= array_job_attuale[i].tipo;
				perm[0].proc_time= array_job_attuale[i].proc_time;
				perm[0].duedate= array_job_attuale[i].deadline;
				perm[0].priority= array_job_attuale[i].priority;
				perm[0].rel_time= array_job_attuale[i].rel_time;
				for(j = 0; j < num_macchine; j++)
				{
					setup_vett[0] = 0;
					disponibilita[i][j] = 0;
					verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
					vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
					vett_setup[i][j] = setup_vett[0];
					if
					(
						
						(
							(array_job_locale[i].duedate != 0)
							&&
							(array_job_locale[i].rel_time ==0)
						)
						||
						(
							(array_job_locale[i].duedate != 0)
							&&					
							(disponibilita[i][j] >= array_job_locale[i].rel_time)
						)
					)
					{
						vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
					}
					else if 
					(
						(array_job_locale[i].duedate == 0)
						&&
						(array_job_locale[i].rel_time <= disponibilita[i][j])
					)
					{
						vett_Lat[i][j] = -63000;
						//indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
					}
					else 					
					{					
						vett_Lat[i][j] = -64000;
					//la release date nn ��?soddisfatta
					}
				}
			}
			else
			{
				for(j = 0; j < num_macchine; j++)
				{
					vett_End[i][j] = 65000;
					vett_Lat[i][j] = -65000;
					vett_setup[i][j] = -65000;
					//segno i job che nn sono disponibili
				}
			}
		}
// 	devo scegliere il job da schedulare
// 	per ogni job scelgo la macchina con Ending time + basso.

		for(i = 0; i < dim_job; i++)
		{
			scelti[i][0]=vett_Lat[i][0];
			int min_C = vett_End[i][0];
			scelti[i][2] = min_C;
			scelti[i][1] = 0;
			for(j = 0; j < num_macchine; j++)
			{
				if(min_C>vett_End[i][j])
				{
					scelti[i][0] = vett_Lat[i][j];
					scelti[i][1] = j;
					scelti[i][2] = vett_End[i][j];
					min_C = vett_End[i][j];
				}
			}
		}
		int max = -65000;
		for(i = 0; i < dim_job; i++)
		{
			if(max < scelti[i][0])
			{
				max = scelti[i][0];

			}
			
		}
		if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
		{
			done_tot = 1;		
		}
		else
		{

			int totale = 0;
			for(i = 0; i < dim_job; i++)//inizializzo
			{
				vett_max_Lat[i] = 0;
			} 
			for(i = 0; i < dim_job; i++)
			{
				if(max == scelti[i][0])
				{
					vett_max_Lat[i] = 1;
					totale++ ;
				}
			}
			
// 			la variabile "totale" mi dice quanti job hanno la medesima lateness
			int coppia[2];
			for(i = 0; i < dim_job; i++)
			{
			
				if(vett_max_Lat[i] == 1)
				{
					coppia[0] = i;
					coppia[1] = scelti[i][1];
					break;
				}
			}
						
			if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
			{
				for(i = 0; i < dim_job; i++)
				{
					
					if(scelti[i][2] < scelti[coppia[0]][2])
					{
						coppia[0] = i;
						coppia[1] = scelti[i][1];
					}
				}					
			}
// 			la coppia i j mi dice quale job schedulare e su quale macchina 
// 			in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
// 			che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
// 			essere cos��?			
			array_job_locale[coppia[0]].schedulato = 1;
			
			perm1[kk] = array_job_attuale[coppia[0]];
			perm[0].ID= array_job_attuale[coppia[0]].ID;
			perm[0].tipo= array_job_attuale[coppia[0]].tipo;
			perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
			perm[0].duedate= array_job_attuale[coppia[0]].deadline;
			perm[0].priority= array_job_attuale[coppia[0]].priority;
			perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
			aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
			kk++;
		}
	}
	elimina_schedula(M1_sch_locale);
	if(num_macchine>=2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	for(i=0;i< dim_job;i++)
	{
		free(disponibilita[i]);
		free(vett_Lat[i]);
		free(vett_End[i]);
		free(vett_setup[i]);
		free(scelti[i]);
	}
	free(vett_max_Lat);
	free(scelti);
	free(disponibilita);
	free(perm);
	free(vett_indisp);
	free(vett_Lat);
	free(vett_End);
	free(vett_setup);
	free(schedule_locali);
	free(array_job_locale);
	return perm1;	
}

void inizializza_perutazione_migliore(job *perm_di_passaggio)
{
	int i;
	for(i=0;i<num_job;i++)
	{
		perm_di_passaggio[i]=array_job[i];
	}
}

job *permutazione_LLF_delta_2(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
	int delta = 2;
	elem **vett_indisp;
	vett_indisp=malloc(num_macchine*sizeof(elem *));
	int **vett_Lat;//contiene il valore della lateness per ogni macchina
	int **vett_End;//contiene il valore dell'ending time per ogni macchina
	int **vett_setup;
	
	int i,j,done_tot = 0,k = 0;
	job *perm;
	perm = malloc(sizeof(job));
	int **disponibilita;
	int **scelti;
	int *vett_max_Lat;
	vett_max_Lat = malloc(dim_job*sizeof(int));
	disponibilita = malloc(dim_job*sizeof(int *));
	vett_Lat = malloc(dim_job*sizeof(int *));
	vett_End = malloc(dim_job*sizeof(int *));
	vett_setup = malloc(dim_job*sizeof(int *));
	scelti = malloc(dim_job*sizeof(int *));
	
	for(i=0;i<dim_job;i++)
	{
		disponibilita[i]=malloc(num_macchine*sizeof(int));
		vett_Lat[i]=malloc(num_macchine*sizeof(int));
		vett_End[i]=malloc(num_macchine*sizeof(int));
		vett_setup[i]=malloc(num_macchine*sizeof(int));
		scelti[i]=malloc(num_macchine*sizeof(int));
	}
	schedula *M1_sch_locale;
	schedula *M2_sch_locale;
	schedula *M3_sch_locale;
	schedula **schedule_locali;
	schedule_locali=malloc(num_macchine*sizeof(schedula*));
	job * perm1;
	job1 *array_job_locale;	
	
	perm1 = malloc(dim_job*sizeof(job));
	array_job_locale=malloc(dim_job*sizeof(job1));
	{//inizializzo le schedule locali
		M1_sch_locale = malloc(sizeof(schedula));
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine>=2)
		{
			M2_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			M3_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	if(num_macchine	== 1)
	{
		vett_indisp[0] = M1;
	}
	if(num_macchine == 2)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
	}
	if(num_macchine == 3)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
		vett_indisp[2] = M3;
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	int kk=0;
	while(done_tot==0)
	{
		int *setup_vett;
		int st_vt;
		setup_vett = &st_vt;
		for(i = 0; i < dim_job; i++)
		{
			if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?	
			{
				perm[0].ID= array_job_attuale[i].ID;
				perm[0].tipo= array_job_attuale[i].tipo;
				perm[0].proc_time= array_job_attuale[i].proc_time;
				perm[0].duedate= array_job_attuale[i].deadline;
				perm[0].priority= array_job_attuale[i].priority;
				perm[0].rel_time= array_job_attuale[i].rel_time;
				for(j = 0; j < num_macchine; j++)
				{
					setup_vett[0] = 0;
					disponibilita[i][j] = 0;
					verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
					vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
					vett_setup[i][j] = setup_vett[0];
					if
					(
						
						(
							(array_job_locale[i].duedate != 0)
							&&
							(array_job_locale[i].rel_time ==0)
						)
						||
						(
							(array_job_locale[i].duedate != 0)
							&&					
							(disponibilita[i][j] >= array_job_locale[i].rel_time)
						)
					)
					{
						vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
					}
					else if 
					(
						(array_job_locale[i].duedate == 0)
						&&
						(array_job_locale[i].rel_time <= disponibilita[i][j])
					)
					{
						vett_Lat[i][j] = -63000;
						//indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
					}
					else 					
					{					
						vett_Lat[i][j] = -64000;
					//la release date nn ��?soddisfatta
					}
				}
			}
			else
			{
				for(j = 0; j < num_macchine; j++)
				{
					vett_End[i][j] = 65000;
					vett_Lat[i][j] = -65000;
					vett_setup[i][j] = -65000;
					//segno i job che nn sono disponibili
				}
			}
		}
// 	devo scegliere il job da schedulare
// 	per ogni job scelgo la macchina con Ending time + basso.
		for(i = 0; i < dim_job; i++)
		{
			scelti[i][0]=vett_Lat[i][0];
			int min_C = vett_End[i][0];
			scelti[i][2] = min_C;
			scelti[i][1] = 0;
			for(j = 0; j < num_macchine; j++)
			{
				if(min_C>vett_End[i][j])
				{
					scelti[i][0] = vett_Lat[i][j];
					scelti[i][1] = j;
					scelti[i][2] = vett_End[i][j];
					min_C = vett_End[i][j];
				}
			}
		}
		int max = -65000;
		for(i = 0; i < dim_job; i++)
		{
			if(max < scelti[i][0])
			{
				max = scelti[i][0];

			}
			
		}
		if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
		{
			done_tot = 1;		
		}
		else
		{
			int totale = 0;
			for(i = 0; i < dim_job; i++)//inizializzo
			{
				vett_max_Lat[i] = 0;
			} 
			for(i = 0; i < dim_job; i++)
			{
				if(max-delta <= scelti[i][0])
				{
					vett_max_Lat[i] = 1;
					totale++ ;
				}
			}
			
// 			la variabile "totale" mi dice quanti job hanno la medesima lateness
			int coppia[2];
			for(i = 0; i < dim_job; i++)
			{
			
				if(vett_max_Lat[i] == 1)
				{
					coppia[0] = i;
					coppia[1] = scelti[i][1];
					break;
				}
			}
						
			if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
			{
				for(i = 0; i < dim_job; i++)
				{
					
					if(scelti[i][2] < scelti[coppia[0]][2])
					{
						coppia[0] = i;
						coppia[1] = scelti[i][1];
					}
				}					
			}
// 			la coppia i j mi dice quale job schedulare e su quale macchina 
// 			in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
// 			che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
// 			essere cos��?	
             		array_job_locale[coppia[0]].schedulato = 1;
			
			perm1[kk] = array_job_attuale[coppia[0]];
			perm[0].ID= array_job_attuale[coppia[0]].ID;
			perm[0].tipo= array_job_attuale[coppia[0]].tipo;
			perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
			perm[0].duedate= array_job_attuale[coppia[0]].deadline;
			perm[0].priority= array_job_attuale[coppia[0]].priority;
			perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
			aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
			kk++;
		}
	}
	elimina_schedula(M1_sch_locale);
	if(num_macchine>=2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	for(i=0;i< dim_job;i++)
	{
		free(disponibilita[i]);
		free(vett_Lat[i]);
		free(vett_End[i]);
		free(vett_setup[i]);
		free(scelti[i]);
	}
	free(vett_max_Lat);
	free(scelti);
	free(disponibilita);
	free(perm);
	free(vett_indisp);
	free(vett_Lat);
	free(vett_End);
	free(vett_setup);
	free(schedule_locali);
	free(array_job_locale);
	return perm1;	
	
}
job *permutazione_LLF_delta_5(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
	int delta = 5;
	elem **vett_indisp;
	vett_indisp=malloc(num_macchine*sizeof(elem *));
	int **vett_Lat;//contiene il valore della lateness per ogni macchina
	int **vett_End;//contiene il valore dell'ending time per ogni macchina
	int **vett_setup;
	
	int i,j,done_tot = 0,k = 0;
	job *perm;
	perm = malloc(sizeof(job));
	int **disponibilita;
	int **scelti;
	int *vett_max_Lat;
	vett_max_Lat = malloc(dim_job*sizeof(int));
	disponibilita = malloc(dim_job*sizeof(int *));
	vett_Lat = malloc(dim_job*sizeof(int *));
	vett_End = malloc(dim_job*sizeof(int *));
	vett_setup = malloc(dim_job*sizeof(int *));
	scelti = malloc(dim_job*sizeof(int *));
	
	for(i=0;i<dim_job;i++)
	{
		disponibilita[i]=malloc(num_macchine*sizeof(int));
		vett_Lat[i]=malloc(num_macchine*sizeof(int));
		vett_End[i]=malloc(num_macchine*sizeof(int));
		vett_setup[i]=malloc(num_macchine*sizeof(int));
		scelti[i]=malloc(num_macchine*sizeof(int));
	}
	schedula *M1_sch_locale;
	schedula *M2_sch_locale;
	schedula *M3_sch_locale;
	schedula **schedule_locali;
	schedule_locali=malloc(num_macchine*sizeof(schedula*));
	job * perm1;
	job1 *array_job_locale;	
	
	perm1 = malloc(dim_job*sizeof(job));
	array_job_locale=malloc(dim_job*sizeof(job1));
	{//inizializzo le schedule locali
		M1_sch_locale = malloc(sizeof(schedula));
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		if(num_macchine>=2)
		{
			M2_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M2_sch,M2_sch_locale);
			schedule_locali[1] = M2_sch_locale;
		}
		if(num_macchine == 3)
		{
			M3_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	if(num_macchine == 1)
	{
		vett_indisp[0] = M1;
	}
	if(num_macchine == 2)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
	}
	if(num_macchine == 3)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
		vett_indisp[2] = M3;
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	int kk=0;
	while(done_tot==0)
	{
		int *setup_vett;
		int st_vt;
		setup_vett = &st_vt;	
		for(i = 0; i < dim_job; i++)
		{
			if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?		
			{
				perm[0].ID= array_job_attuale[i].ID;
				perm[0].tipo= array_job_attuale[i].tipo;
				perm[0].proc_time= array_job_attuale[i].proc_time;
				perm[0].duedate= array_job_attuale[i].deadline;
				perm[0].priority= array_job_attuale[i].priority;
				perm[0].rel_time= array_job_attuale[i].rel_time;
				for(j = 0; j < num_macchine; j++)
				{
					setup_vett[0] = 0;
					disponibilita[i][j] = 0;
					verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
					vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
					vett_setup[i][j] = setup_vett[0];
					if
					(
						
						(
							(array_job_locale[i].duedate != 0)
							&&
							(array_job_locale[i].rel_time ==0)
						)
						||
						(
							(array_job_locale[i].duedate != 0)
							&&					
							(disponibilita[i][j] >= array_job_locale[i].rel_time)
						)
					)
					{
						vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
					}
					else if 
					(
						(array_job_locale[i].duedate == 0)
						&&
						(array_job_locale[i].rel_time <= disponibilita[i][j])
					)
					{
						vett_Lat[i][j] = -63000;
						//indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
					}
					else 					
					{					
						vett_Lat[i][j] = -64000;
					//la release date nn ��?soddisfatta
					}
				}
			}
			else
			{
				for(j = 0; j < num_macchine; j++)
				{
					vett_End[i][j] = 65000;
					vett_Lat[i][j] = -65000;
					vett_setup[i][j] = -65000;
					//segno i job che nn sono disponibili
				}
			}
		}
// 	devo scegliere il job da schedulare
// 	per ogni job scelgo la macchina con Ending time + basso.

		for(i = 0; i < dim_job; i++)
		{
			scelti[i][0]=vett_Lat[i][0];
			int min_C = vett_End[i][0];
			scelti[i][2] = min_C;
			scelti[i][1] = 0;
			for(j = 0; j < num_macchine; j++)
			{
				if(min_C>vett_End[i][j])
				{
					scelti[i][0] = vett_Lat[i][j];
					scelti[i][1] = j;
					scelti[i][2] = vett_End[i][j];
					min_C = vett_End[i][j];
				}
			}
		}
		int max = -65000;
		for(i = 0; i < dim_job; i++)
		{
			if(max < scelti[i][0])
			{
				max = scelti[i][0];

			}
			
		}
		if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
		{
			done_tot = 1;		
		}
		else
		{

			int totale = 0;
			for(i = 0; i < dim_job; i++)//inizializzo
			{
				vett_max_Lat[i] = 0;
			} 
			for(i = 0; i < dim_job; i++)
			{
				if(max-delta <= scelti[i][0])
				{
					vett_max_Lat[i] = 1;
					totale++ ;
				}
			}
			
// 			la variabile "totale" mi dice quanti job hanno la medesima lateness
			int coppia[2];
			for(i = 0; i < dim_job; i++)
			{
			
				if(vett_max_Lat[i] == 1)
				{
					coppia[0] = i;
					coppia[1] = scelti[i][1];
					break;
				}
			}
						
			if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
			{
				for(i = 0; i < dim_job; i++)
				{
					
					if(scelti[i][2] < scelti[coppia[0]][2])
					{
						coppia[0] = i;
						coppia[1] = scelti[i][1];
					}
				}					
			}
// 			la coppia i j mi dice quale job schedulare e su quale macchina 
// 			in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
// 			che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
// 			essere cos��?		
                	array_job_locale[coppia[0]].schedulato = 1;
			
			perm1[kk] = array_job_attuale[coppia[0]];
			perm[0].ID= array_job_attuale[coppia[0]].ID;
			perm[0].tipo= array_job_attuale[coppia[0]].tipo;
			perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
			perm[0].duedate= array_job_attuale[coppia[0]].deadline;
			perm[0].priority= array_job_attuale[coppia[0]].priority;
			perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
			aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
			kk++;
		}
	}
	elimina_schedula(M1_sch_locale);
	if(num_macchine>=2)
	{
		elimina_schedula(M2_sch_locale);
 	}
	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	for(i=0;i< dim_job;i++)
	{
		free(disponibilita[i]);
		free(vett_Lat[i]);
		free(vett_End[i]);
		free(vett_setup[i]);
		free(scelti[i]);
	}
	free(vett_max_Lat);
	free(scelti);
	free(disponibilita);
	free(perm);
	free(vett_indisp);
	free(vett_Lat);
	free(vett_End);
	free(vett_setup);
	free(schedule_locali);
	free(array_job_locale);
	return perm1;
}
job *permutazione_LLF_delta_10(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
	int delta = 10;
	elem **vett_indisp;
	vett_indisp=malloc(num_macchine*sizeof(elem *));
	int **vett_Lat;//contiene il valore della lateness per ogni macchina
	int **vett_End;//contiene il valore dell'ending time per ogni macchina
	int **vett_setup;
	
	int i,j,done_tot = 0,k = 0;
	job *perm;
	perm = malloc(sizeof(job));
	int **disponibilita;
	int **scelti;
	int *vett_max_Lat;
	vett_max_Lat = malloc(dim_job*sizeof(int));
	disponibilita = malloc(dim_job*sizeof(int *));
	vett_Lat = malloc(dim_job*sizeof(int *));
	vett_End = malloc(dim_job*sizeof(int *));
	vett_setup = malloc(dim_job*sizeof(int *));
	scelti = malloc(dim_job*sizeof(int *));
	
	for(i=0;i<dim_job;i++)
	{
		disponibilita[i]=malloc(num_macchine*sizeof(int));
		vett_Lat[i]=malloc(num_macchine*sizeof(int));
		vett_End[i]=malloc(num_macchine*sizeof(int));
		vett_setup[i]=malloc(num_macchine*sizeof(int));
		scelti[i]=malloc(num_macchine*sizeof(int));
	}
	schedula *M1_sch_locale;
	schedula *M2_sch_locale;
	schedula *M3_sch_locale;
	schedula **schedule_locali;
	schedule_locali=malloc(num_macchine*sizeof(schedula*));
	job * perm1;
	job1 *array_job_locale;	
	
	perm1 = malloc(dim_job*sizeof(job));
	array_job_locale=malloc(dim_job*sizeof(job1));
	{//inizializzo le schedule locali
		M1_sch_locale = malloc(sizeof(schedula));
		copia_schelule(M1_sch,M1_sch_locale);	
		schedule_locali[0] = M1_sch_locale;
		M2_sch_locale = malloc(sizeof(schedula));
		copia_schelule(M2_sch,M2_sch_locale);
		schedule_locali[1] = M2_sch_locale;
		if(num_macchine == 3)
		{
			M3_sch_locale = malloc(sizeof(schedula));
			copia_schelule(M3_sch,M3_sch_locale);
			schedule_locali[2] = M3_sch_locale;
		}
	}
	if(num_macchine == 2)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
	}
	if(num_macchine == 3)
	{
		vett_indisp[0] = M1;
		vett_indisp[1] = M2;
		vett_indisp[2] = M3;
	}
	for(i = 0;i<dim_job;i++)
	{
		array_job_locale[i].ID = array_job_attuale[i].ID;
		array_job_locale[i].tipo = array_job_attuale[i].tipo;
		array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
		array_job_locale[i].duedate = array_job_attuale[i].duedate;
		array_job_locale[i].deadline = array_job_attuale[i].deadline;
		array_job_locale[i].priority = array_job_attuale[i].priority;
		array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
		array_job_locale[i].adatto = 0;
		array_job_locale[i].schedulato = 0;
	}
	int kk=0;
	while(done_tot==0)
	{
		int *setup_vett;
		int st_vt;
		setup_vett = &st_vt;
		for(i = 0; i < dim_job; i++)
		{
			if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?			
			{
				perm[0].ID= array_job_attuale[i].ID;
				perm[0].tipo= array_job_attuale[i].tipo;
				perm[0].proc_time= array_job_attuale[i].proc_time;
				perm[0].duedate= array_job_attuale[i].deadline;
				perm[0].priority= array_job_attuale[i].priority;
				perm[0].rel_time= array_job_attuale[i].rel_time;
				for(j = 0; j < num_macchine; j++)
				{
					setup_vett[0] = 0;
					disponibilita[i][j] = 0;
					verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
					vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
					vett_setup[i][j] = setup_vett[0];
					if
					(
						
						(
							(array_job_locale[i].duedate != 0)
							&&
							(array_job_locale[i].rel_time ==0)
						)
						||
						(
							(array_job_locale[i].duedate != 0)
							&&					
							(disponibilita[i][j] <= array_job_locale[i].rel_time)
						)
					)
					{
						vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
					}
					else if 
					(
						(array_job_locale[i].duedate == 0)
						&&
						(array_job_locale[i].rel_time == 0)
					)
					{
						vett_Lat[i][j] = -63000;
						//indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
					}
					else 					
					{					
						vett_Lat[i][j] = -64000;
					//la release date nn ��?soddisfatta
					}
				}
			}
			else
			{
				for(j = 0; j < num_macchine; j++)
				{
					vett_End[i][j] = 65000;
					vett_Lat[i][j] = -65000;
					vett_setup[i][j] = -65000;
					//segno i job che nn sono disponibili
				}
			}
		}
// 	devo scegliere il job da schedulare
// 	per ogni job scelgo la macchina con Ending time + basso.

		for(i = 0; i < dim_job; i++)
		{
			scelti[i][0]=vett_Lat[i][0];
			int min_C = vett_End[i][0];
			scelti[i][2] = min_C;
			scelti[i][1] = 0;
			for(j = 0; j < num_macchine; j++)
			{
				if(min_C>vett_End[i][j])
				{
					scelti[i][0] = vett_Lat[i][j];
					scelti[i][1] = j;
					scelti[i][2] = vett_End[i][j];
					min_C = vett_End[i][j];
				}
			}
		}
		int max = -65000;
		for(i = 0; i < dim_job; i++)
		{
			if(max < scelti[i][0])
			{
				max = scelti[i][0];
			}
			
		}
		if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
		{
			done_tot = 1;		
		}
		else
		{
			int totale = 0;
			for(i = 0; i < dim_job; i++)//inizializzo
			{
				vett_max_Lat[i] = 0;
			} 
			for(i = 0; i < dim_job; i++)
			{
				if(max-delta <= scelti[i][0])
				{
					vett_max_Lat[i] = 1;
					totale++ ;
				}
			}
			
// 			la variabile "totale" mi dice quanti job hanno la medesima lateness
			int coppia[2];
			for(i = 0; i < dim_job; i++)
			{
			
				if(vett_max_Lat[i] == 1)
				{
					coppia[0] = i;
					coppia[1] = scelti[i][1];
					break;
				}
			}
						
			if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
			{
				for(i = 0; i < dim_job; i++)
				{
					
					if(scelti[i][2] < scelti[coppia[0]][2])
					{
						coppia[0] = i;
						coppia[1] = scelti[i][1];
					}
				}					
			}
// 			la coppia i j mi dice quale job schedulare e su quale macchina 
// 			in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
// 			che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
// 			essere cos��?
			array_job_locale[coppia[0]].schedulato = 1;
			perm1[kk] = array_job_attuale[coppia[0]];
			aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
			kk++;
		}
	}
	elimina_schedula(M1_sch_locale);
	elimina_schedula(M2_sch_locale);
 	if(num_macchine == 3)
	{
		elimina_schedula(M3_sch_locale);
	}
	
	for(i=0;i< dim_job;i++)
	{
		free(disponibilita[i]);
		free(vett_Lat[i]);
		free(vett_End[i]);
		free(vett_setup[i]);
		free(scelti[i]);
	}
	free(vett_max_Lat);
	free(scelti);
	free(disponibilita);
	free(perm);
	free(vett_indisp);
	free(vett_Lat);
	free(vett_End);
	free(vett_setup);
	free(schedule_locali);
	free(array_job_locale);
	return perm1;	
}


/****************************************************************/
/* questa funzione verifica che minimum non si trovi 		*/
/* all'interno di un periodo di indisponibilita' ed in tal caso */
/* restituisce l'estremo superiore di tale periodo		*/
/****************************************************************/
int trova_edge_indisp(elem *M,int minimum)
{
	elem * temp;
	temp=M;
	while(temp->next!=NULL)
	{
		if(temp->inizio>minimum)
		{
			return minimum;
		}
		else if(temp->fine>=minimum)
		{
			return temp->fine;
		}
		else
		{
			temp=temp->next;
		}
	}
	return minimum;	// nel caso in cui non ci siano indisponibilita'
			// successive a minimum
	
}

int calcola_numero_eur(int tipo_eur)
{
    if(tipo_eur==15)
    {
        return 2;
    }
    else if(tipo_eur==16)
    {
	return 10;
        //return 3;
    }
    else if(tipo_eur==17)
    {
        //return 16;
	return 15;
    }
    else
    {
        return 1;
    }
}
int calcola_proc_time_medio(void)
{
//     questa funzione calcola il proc. time medio 
    int i=0;
    int totale=0;
    int temp;
    for(i=0;i<num_job;i++)
    {
        totale+=array_job[i].proc_time;
    }
    temp = ceil((float)totale/num_job);
    return temp;
}
job *permutazione_LLF_delta_7ore(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 420;
    elem **vett_indisp;
    vett_indisp=malloc(num_macchine*sizeof(elem *));
    int **vett_Lat;//contiene il valore della lateness per ogni macchina
    int **vett_End;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup;
   
    int i,j,done_tot = 0,k = 0;
    job *perm;
    perm = malloc(sizeof(job));
    int **disponibilita;
    int **scelti;
    int *vett_max_Lat;
    vett_max_Lat = malloc(dim_job*sizeof(int));
    disponibilita = malloc(dim_job*sizeof(int *));
    vett_Lat = malloc(dim_job*sizeof(int *));
    vett_End = malloc(dim_job*sizeof(int *));
    vett_setup = malloc(dim_job*sizeof(int *));
    scelti = malloc(dim_job*sizeof(int *));
   
    for(i=0;i<dim_job;i++)
    {
        disponibilita[i]=malloc(num_macchine*sizeof(int));
        vett_Lat[i]=malloc(num_macchine*sizeof(int));
        vett_End[i]=malloc(num_macchine*sizeof(int));
        vett_setup[i]=malloc(num_macchine*sizeof(int));
        scelti[i]=malloc(num_macchine*sizeof(int));
    }
    schedula *M1_sch_locale;
    schedula *M2_sch_locale;
    schedula *M3_sch_locale;
    schedula **schedule_locali;
    schedule_locali=malloc(num_macchine*sizeof(schedula*));
    job * perm1;
    job1 *array_job_locale; 
   
    perm1 = malloc(dim_job*sizeof(job));
    array_job_locale=malloc(dim_job*sizeof(job1));
    {//inizializzo le schedule locali
        M1_sch_locale = malloc(sizeof(schedula));
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine>=2)
        {
            M2_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            M3_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(num_macchine   == 1)
    {
        vett_indisp[0] = M1;
    }
    if(num_macchine == 2)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
    }
    if(num_macchine == 3)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
        vett_indisp[2] = M3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?      
	      {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < num_macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (
                  
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&             
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if 
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
                    }
                    else              
                    {              
                        vett_Lat[i][j] = -64000;
               //la release date nn ��?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < num_macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < num_macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
            {
                max = scelti[i][0];

            }
         
        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
        {
            done_tot = 1;     
        }
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            } 
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }
         
//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {
         
                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }
                  
            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {
               
                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }              
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina 
//          in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
//          essere cos��?
            array_job_locale[coppia[0]].schedulato = 1;
         
            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    elimina_schedula(M1_sch_locale);
    if(num_macchine>=2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    for(i=0;i< dim_job;i++)
    {
        free(disponibilita[i]);
        free(vett_Lat[i]);
        free(vett_End[i]);
        free(vett_setup[i]);
        free(scelti[i]);
    }
    free(vett_max_Lat);
    free(scelti);
    free(disponibilita);
    free(perm);
    free(vett_indisp);
    free(vett_Lat);
    free(vett_End);
    free(vett_setup);
    free(schedule_locali);
    free(array_job_locale);
    return perm1;  
   
}
job *permutazione_LLF_delta_24ore(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 840;
    elem **vett_indisp;
    vett_indisp=malloc(num_macchine*sizeof(elem *));
    int **vett_Lat;//contiene il valore della lateness per ogni macchina
    int **vett_End;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup;
   
    int i,j,done_tot = 0,k = 0;
    job *perm;
    perm = malloc(sizeof(job));
    int **disponibilita;
    int **scelti;
    int *vett_max_Lat;
    vett_max_Lat = malloc(dim_job*sizeof(int));
    disponibilita = malloc(dim_job*sizeof(int *));
    vett_Lat = malloc(dim_job*sizeof(int *));
    vett_End = malloc(dim_job*sizeof(int *));
    vett_setup = malloc(dim_job*sizeof(int *));
    scelti = malloc(dim_job*sizeof(int *));
   
    for(i=0;i<dim_job;i++)
    {
        disponibilita[i]=malloc(num_macchine*sizeof(int));
        vett_Lat[i]=malloc(num_macchine*sizeof(int));
        vett_End[i]=malloc(num_macchine*sizeof(int));
        vett_setup[i]=malloc(num_macchine*sizeof(int));
        scelti[i]=malloc(num_macchine*sizeof(int));
    }
    schedula *M1_sch_locale;
    schedula *M2_sch_locale;
    schedula *M3_sch_locale;
    schedula **schedule_locali;
    schedule_locali=malloc(num_macchine*sizeof(schedula*));
    job * perm1;
    job1 *array_job_locale; 
   
    perm1 = malloc(dim_job*sizeof(job));
    array_job_locale=malloc(dim_job*sizeof(job1));
    {//inizializzo le schedule locali
        M1_sch_locale = malloc(sizeof(schedula));
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine>=2)
        {
            M2_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            M3_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(num_macchine   == 1)
    {
        vett_indisp[0] = M1;
    }
    if(num_macchine == 2)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
    }
    if(num_macchine == 3)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
        vett_indisp[2] = M3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?      
	      {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < num_macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (
                  
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&             
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if 
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
                    }
                    else              
                    {              
                        vett_Lat[i][j] = -64000;
               //la release date nn ��?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < num_macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < num_macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
            {
                max = scelti[i][0];

            }
         
        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
        {
            done_tot = 1;     
        }
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            } 
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }
         
//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {
         
                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }
                  
            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {
               
                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }              
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina 
//          in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
//          essere cos��?         
            array_job_locale[coppia[0]].schedulato = 1;
         
            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    elimina_schedula(M1_sch_locale);
    if(num_macchine>=2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    for(i=0;i< dim_job;i++)
    {
        free(disponibilita[i]);
        free(vett_Lat[i]);
        free(vett_End[i]);
        free(vett_setup[i]);
        free(scelti[i]);
    }
    free(vett_max_Lat);
    free(scelti);
    free(disponibilita);
    free(perm);
    free(vett_indisp);
    free(vett_Lat);
    free(vett_End);
    free(vett_setup);
    free(schedule_locali);
    free(array_job_locale);
    return perm1;  
   
}
job *permutazione_LLF_delta_3_proc_medio(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 0;
    elem **vett_indisp;
    vett_indisp=malloc(num_macchine*sizeof(elem *));
    int **vett_Lat;//contiene il valore della lateness per ogni macchina
    int **vett_End;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup;
   
    int i,j,done_tot = 0,k = 0;
    job *perm;
    perm = malloc(sizeof(job));
    int **disponibilita;
    int **scelti;
    int *vett_max_Lat;
    delta = 3*calcola_proc_time_medio(); 
    vett_max_Lat = malloc(dim_job*sizeof(int));
    disponibilita = malloc(dim_job*sizeof(int *));
    vett_Lat = malloc(dim_job*sizeof(int *));
    vett_End = malloc(dim_job*sizeof(int *));
    vett_setup = malloc(dim_job*sizeof(int *));
    scelti = malloc(dim_job*sizeof(int *));
   
    for(i=0;i<dim_job;i++)
    {
        disponibilita[i]=malloc(num_macchine*sizeof(int));
        vett_Lat[i]=malloc(num_macchine*sizeof(int));
        vett_End[i]=malloc(num_macchine*sizeof(int));
        vett_setup[i]=malloc(num_macchine*sizeof(int));
        scelti[i]=malloc(num_macchine*sizeof(int));
    }
    schedula *M1_sch_locale;
    schedula *M2_sch_locale;
    schedula *M3_sch_locale;
    schedula **schedule_locali;
    schedule_locali=malloc(num_macchine*sizeof(schedula*));
    job * perm1;
    job1 *array_job_locale; 
   
    perm1 = malloc(dim_job*sizeof(job));
    array_job_locale=malloc(dim_job*sizeof(job1));
    {//inizializzo le schedule locali
        M1_sch_locale = malloc(sizeof(schedula));
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine>=2)
        {
            M2_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            M3_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(num_macchine   == 1)
    {
        vett_indisp[0] = M1;
    }
    if(num_macchine == 2)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
    }
    if(num_macchine == 3)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
        vett_indisp[2] = M3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?          
	    {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < num_macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (
                  
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&             
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if 
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
                    }
                    else              
                    {              
                        vett_Lat[i][j] = -64000;
               //la release date nn ��?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < num_macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < num_macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
            {
                max = scelti[i][0];

            }
         
        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
        {
            done_tot = 1;     
        }
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            } 
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }
         
//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {
         
                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }
                  
            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {
               
                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }              
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina 
//          in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
//          essere cos��?      
            array_job_locale[coppia[0]].schedulato = 1;
         
            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    elimina_schedula(M1_sch_locale);
    if(num_macchine>=2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    for(i=0;i< dim_job;i++)
    {
        free(disponibilita[i]);
        free(vett_Lat[i]);
        free(vett_End[i]);
        free(vett_setup[i]);
        free(scelti[i]);
    }
    free(vett_max_Lat);
    free(scelti);
    free(disponibilita);
    free(perm);
    free(vett_indisp);
    free(vett_Lat);
    free(vett_End);
    free(vett_setup);
    free(schedule_locali);
    free(array_job_locale);
    return perm1;  
   
}
job *permutazione_LLF_delta_proc_medio(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 0;
    elem **vett_indisp;
    vett_indisp=malloc(num_macchine*sizeof(elem *));
    int **vett_Lat;//contiene il valore della lateness per ogni macchina
    int **vett_End;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup;
   
    int i,j,done_tot = 0,k = 0;
    job *perm;
    perm = malloc(sizeof(job));
    int **disponibilita;
    int **scelti;
    int *vett_max_Lat;
    delta = calcola_proc_time_medio(); 
    vett_max_Lat = malloc(dim_job*sizeof(int));
    disponibilita = malloc(dim_job*sizeof(int *));
    vett_Lat = malloc(dim_job*sizeof(int *));
    vett_End = malloc(dim_job*sizeof(int *));
    vett_setup = malloc(dim_job*sizeof(int *));
    scelti = malloc(dim_job*sizeof(int *));
   
    for(i=0;i<dim_job;i++)
    {
        disponibilita[i]=malloc(num_macchine*sizeof(int));
        vett_Lat[i]=malloc(num_macchine*sizeof(int));
        vett_End[i]=malloc(num_macchine*sizeof(int));
        vett_setup[i]=malloc(num_macchine*sizeof(int));
        scelti[i]=malloc(num_macchine*sizeof(int));
    }
    schedula *M1_sch_locale;
    schedula *M2_sch_locale;
    schedula *M3_sch_locale;
    schedula **schedule_locali;
    schedule_locali=malloc(num_macchine*sizeof(schedula*));
    job * perm1;
    job1 *array_job_locale; 
   
    perm1 = malloc(dim_job*sizeof(job));
    array_job_locale=malloc(dim_job*sizeof(job1));
    {//inizializzo le schedule locali
        M1_sch_locale = malloc(sizeof(schedula));
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine>=2)
        {
            M2_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            M3_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(num_macchine   == 1)
    {
        vett_indisp[0] = M1;
    }
    if(num_macchine == 2)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
    }
    if(num_macchine == 3)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
        vett_indisp[2] = M3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?            
	    {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < num_macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (
                  
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&             
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if 
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
                    }
                    else              
                    {              
                        vett_Lat[i][j] = -64000;
               //la release date nn ��?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < num_macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < num_macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
            {
                max = scelti[i][0];

            }
         
        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
        {
            done_tot = 1;     
        }
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            } 
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }
         
//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {
         
                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }
                  
            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {
               
                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }              
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina 
//          in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
//          essere cos��?           

            array_job_locale[coppia[0]].schedulato = 1;
            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    elimina_schedula(M1_sch_locale);
    if(num_macchine>=2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    for(i=0;i< dim_job;i++)
    {
        free(disponibilita[i]);
        free(vett_Lat[i]);
        free(vett_End[i]);
        free(vett_setup[i]);
        free(scelti[i]);
    }
    free(vett_max_Lat);
    free(scelti);
    free(disponibilita);
    free(perm);
    free(vett_indisp);
    free(vett_Lat);
    free(vett_End);
    free(vett_setup);
    free(schedule_locali);
    free(array_job_locale);
    return perm1;  
   
}
job *permutazione_LLF_delta_mezzo_proc_medio(job *array_job_attuale, int dim_job) 
{
// per ogni macchina verifico ogni job e poi scelgo quello con lateness + alta
// tra quelli equivalenti si sceglier��?il job con ending time + basso
    int delta = 0;
    elem **vett_indisp;
    vett_indisp=malloc(num_macchine*sizeof(elem *));
    int **vett_Lat;//contiene il valore della lateness per ogni macchina
    int **vett_End;//contiene il valore dell'ending time per ogni macchina
    int **vett_setup;
   
    int i,j,done_tot = 0,k = 0;
    job *perm;
    perm = malloc(sizeof(job));
    int **disponibilita;
    int **scelti;
    int *vett_max_Lat;
    delta = ceil((float)calcola_proc_time_medio()/2); 
    vett_max_Lat = malloc(dim_job*sizeof(int));
    disponibilita = malloc(dim_job*sizeof(int *));
    vett_Lat = malloc(dim_job*sizeof(int *));
    vett_End = malloc(dim_job*sizeof(int *));
    vett_setup = malloc(dim_job*sizeof(int *));
    scelti = malloc(dim_job*sizeof(int *));
   
    for(i=0;i<dim_job;i++)
    {
        disponibilita[i]=malloc(num_macchine*sizeof(int));
        vett_Lat[i]=malloc(num_macchine*sizeof(int));
        vett_End[i]=malloc(num_macchine*sizeof(int));
        vett_setup[i]=malloc(num_macchine*sizeof(int));
        scelti[i]=malloc(num_macchine*sizeof(int));
    }
    schedula *M1_sch_locale;
    schedula *M2_sch_locale;
    schedula *M3_sch_locale;
    schedula **schedule_locali;
    schedule_locali=malloc(num_macchine*sizeof(schedula*));
    job * perm1;
    job1 *array_job_locale; 
   
    perm1 = malloc(dim_job*sizeof(job));
    array_job_locale=malloc(dim_job*sizeof(job1));
    {//inizializzo le schedule locali
        M1_sch_locale = malloc(sizeof(schedula));
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine>=2)
        {
            M2_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            M3_sch_locale = malloc(sizeof(schedula));
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    if(num_macchine   == 1)
    {
        vett_indisp[0] = M1;
    }
    if(num_macchine == 2)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
    }
    if(num_macchine == 3)
    {
        vett_indisp[0] = M1;
        vett_indisp[1] = M2;
        vett_indisp[2] = M3;
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    int kk=0;
    while(done_tot==0)
    {
        int *setup_vett;
        int st_vt;
        setup_vett = &st_vt;
        for(i = 0; i < dim_job; i++)
        {
            if(array_job_locale[i].schedulato == 0)//se nn ��?schedulato gi��?            
	    {
                perm[0].ID= array_job_attuale[i].ID;
                perm[0].tipo= array_job_attuale[i].tipo;
                perm[0].proc_time= array_job_attuale[i].proc_time;
                perm[0].duedate= array_job_attuale[i].deadline;
                perm[0].priority= array_job_attuale[i].priority;
                perm[0].rel_time= array_job_attuale[i].rel_time;
                for(j = 0; j < num_macchine; j++)
                {
                    setup_vett[0] = 0;
                    disponibilita[i][j] = 0;
                    verifica_macchina(schedule_locali[j],vett_indisp[j],&disponibilita[i][j],setup_vett,0,perm,0);
                    vett_End[i][j] = disponibilita[i][j] + array_job_attuale[i].proc_time;
                    vett_setup[i][j] = setup_vett[0];
                    if
                    (
                  
                     (
                     (array_job_locale[i].duedate != 0)
                     &&
                     (array_job_locale[i].rel_time ==0)
                     )
                     ||
                     (
                     (array_job_locale[i].duedate != 0)
                     &&             
                     (disponibilita[i][j] >= array_job_locale[i].rel_time)
                     )
                    )
                    {
                        vett_Lat[i][j] = vett_End[i][j] - array_job_locale[i].duedate;
                    }
                    else if 
                    (
                     (array_job_locale[i].duedate == 0)
                     &&
                     (array_job_locale[i].rel_time <= disponibilita[i][j])
                    )
                    {
                        vett_Lat[i][j] = -63000;
                  //indica che nn c'��?duedate e quindi vanno schedulati per ultimi 
                    }
                    else              
                    {              
                        vett_Lat[i][j] = -64000;
               //la release date nn ��?soddisfatta
                    }
                }
            }
            else
            {
                for(j = 0; j < num_macchine; j++)
                {
                    vett_End[i][j] = 65000;
                    vett_Lat[i][j] = -65000;
                    vett_setup[i][j] = -65000;
               //segno i job che nn sono disponibili
                }
            }
        }
//    devo scegliere il job da schedulare
//    per ogni job scelgo la macchina con Ending time + basso.
        for(i = 0; i < dim_job; i++)
        {
            scelti[i][0]=vett_Lat[i][0];
            int min_C = vett_End[i][0];
            scelti[i][2] = min_C;
            scelti[i][1] = 0;
            for(j = 0; j < num_macchine; j++)
            {
                if(min_C>vett_End[i][j])
                {
                    scelti[i][0] = vett_Lat[i][j];
                    scelti[i][1] = j;
                    scelti[i][2] = vett_End[i][j];
                    min_C = vett_End[i][j];
                }
            }
        }
        int max = -65000;
        for(i = 0; i < dim_job; i++)
        {
            if(max < scelti[i][0])
            {
                max = scelti[i][0];

            }
         
        }
        if(max == -65000) // vuol dire che nn ci sono + job da schedulare e quindi ho finito
        {
            done_tot = 1;     
        }
        else
        {
            int totale = 0;
            for(i = 0; i < dim_job; i++)//inizializzo
            {
                vett_max_Lat[i] = 0;
            } 
            for(i = 0; i < dim_job; i++)
            {
                if(max-delta <= scelti[i][0])
                {
                    vett_max_Lat[i] = 1;
                    totale++ ;
                }
            }
         
//          la variabile "totale" mi dice quanti job hanno la medesima lateness
            int coppia[2];
            for(i = 0; i < dim_job; i++)
            {
         
                if(vett_max_Lat[i] == 1)
                {
                    coppia[0] = i;
                    coppia[1] = scelti[i][1];
                    break;
                }
            }
                  
            if(totale>1)//devo cercare la coppia + adatta altrimenti quella che ho mi sta bene
            {
                for(i = 0; i < dim_job; i++)
                {
               
                    if(scelti[i][2] < scelti[coppia[0]][2])
                    {
                        coppia[0] = i;
                        coppia[1] = scelti[i][1];
                    }
                }              
            }
//          la coppia i j mi dice quale job schedulare e su quale macchina 
//          in realt��?costruiamo solo la sequenza di job e nn possiamo essere sicuri che 
//          che il job prescelto venga schedulato sulla macchina prescelta anche se ad occhio e croce dovrebbe 
//          essere cos��?          

             array_job_locale[coppia[0]].schedulato = 1;
         
            perm1[kk] = array_job_attuale[coppia[0]];
            perm[0].ID= array_job_attuale[coppia[0]].ID;
            perm[0].tipo= array_job_attuale[coppia[0]].tipo;
            perm[0].proc_time= array_job_attuale[coppia[0]].proc_time;
            perm[0].duedate= array_job_attuale[coppia[0]].deadline;
            perm[0].priority= array_job_attuale[coppia[0]].priority;
            perm[0].rel_time= array_job_attuale[coppia[0]].rel_time;
            aggiungi_schedula(schedule_locali[coppia[1]],perm[0],disponibilita[coppia[0]][coppia[1]],vett_setup[coppia[0]][coppia[1]]);
            kk++;
        }
    }
    elimina_schedula(M1_sch_locale);
    if(num_macchine>=2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    for(i=0;i< dim_job;i++)
    {
        free(disponibilita[i]);
        free(vett_Lat[i]);
        free(vett_End[i]);
        free(vett_setup[i]);
        free(scelti[i]);
    }
    free(vett_max_Lat);
    free(scelti);
    free(disponibilita);
    free(perm);
    free(vett_indisp);
    free(vett_Lat);
    free(vett_End);
    free(vett_setup);
    free(schedule_locali);
    free(array_job_locale);
    return perm1;  
   
}

job *permutazione_SPT_semplice(job *array_job_attuale, int dim_job)
{//questa euristica costruisce la permutazione in base al proc_time del job
	int *proc_time_vett;
	proc_time_vett=malloc(dim_job*sizeof(int));
	int *ID_job_vett;
	ID_job_vett=malloc(dim_job*sizeof(int));
	int i,k;
	job *perm1;
	perm1 = malloc(dim_job*sizeof(job));
	for(i=0;i<dim_job;i++)
	{//carico il valore del proc_time
		proc_time_vett[i]=array_job_attuale[i].proc_time;
		ID_job_vett[i]=array_job_attuale[i].ID;	
	}
	for(k=0;k<dim_job;k++)
	{
		int pos=0;
		i=0;
		while(i<dim_job)
		{
			if(ID_job_vett[i]!=-1)//devo verificare che il job nn sia gi��?stato schedulato
			{
				pos = i;
				break;
			}
			i++;
		}
// 	int min_proc_time = proc_time_vett[pos];
		for(i =1;i<dim_job;i++)
		{
			if
			(
				(ID_job_vett[i]!= -1)
				&&
				(proc_time_vett[i]<proc_time_vett[pos])
			)
			{//sostituisco il valore trovato con quello migliore
				pos = i;// ��?anche la posizione del job in array_job_attuale
			}
			
		}
		ID_job_vett[pos]=-1;//segno come schedulato il job prescelto
		perm1[k]=array_job_attuale[pos];
	}
	free(ID_job_vett);
	free(proc_time_vett);
	return perm1;
}
job *permutazione_delta_mezzo_proc_medio(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    job1 *array_job_locale;    
    int i= 0;
    int Delta = 0;
    int temp = 0;
    schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;
    schedula **schedule_locali;
    job * perm1;
    int k;

    array_job_locale=malloc(dim_job*sizeof(job1));  
    Delta = ceil((float)calcola_proc_time_medio()/2);
       
    M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
    if(num_macchine >= 2)
    {
        M2_sch_locale = malloc(sizeof(schedula));
    }
    if(num_macchine == 3)
    {
        M3_sch_locale = malloc(sizeof(schedula));
    }
    schedule_locali=malloc(num_macchine*sizeof(schedula *));
    perm1 = malloc(dim_job*sizeof(job));
    {//inizializzo le schedule locali
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine >= 2)
        {
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    elimina_schedula(M1_sch_locale);
    if(num_macchine >= 2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    free(schedule_locali);
    free(array_job_locale);
    return perm1;

}
job *permutazione_delta_proc_medio(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    job1 *array_job_locale;
    
    int i= 0;
    int Delta = 0;
    int temp = 0;
    schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;   
    schedula **schedule_locali;
    job * perm1;
    int k;	
    array_job_locale=malloc(dim_job*sizeof(job1));  	
    Delta = calcola_proc_time_medio();
    M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
    if(num_macchine >= 2)
    {
        M2_sch_locale = malloc(sizeof(schedula));
    }
    if(num_macchine == 3)
    {
        M3_sch_locale = malloc(sizeof(schedula));
    }
    schedule_locali=malloc(num_macchine*sizeof(schedula *));
    perm1 = malloc(dim_job*sizeof(job));
    {//inizializzo le schedule locali
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine >= 2)
        {
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    elimina_schedula(M1_sch_locale);
    if(num_macchine >= 2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    free(schedule_locali);
    free(array_job_locale);
    return perm1;

}
job *permutazione_delta_3_proc_medio(job *array_job_attuale, int dim_job)
{//questa e' un'euristica basata sul concetto di Delta
    job1 *array_job_locale;
    int i= 0;
    int Delta = 0;
    int temp = 0;
    schedula *M1_sch_locale,*M2_sch_locale,*M3_sch_locale;   
    schedula **schedule_locali;
    job * perm1;
    int k;	

    Delta = 3*calcola_proc_time_medio();
    array_job_locale=malloc(dim_job*sizeof(job1));  
    M1_sch_locale = malloc(sizeof(schedula));// tali schedule contengono almeno un elemento nullo
    if(num_macchine >= 2)
    {
        M2_sch_locale = malloc(sizeof(schedula));
    }
    if(num_macchine == 3)
    {
        M3_sch_locale = malloc(sizeof(schedula));
    }
    schedule_locali=malloc(num_macchine*sizeof(schedula *));
    perm1 = malloc(dim_job*sizeof(job));
    {//inizializzo le schedule locali
        copia_schelule(M1_sch,M1_sch_locale);  
        schedule_locali[0] = M1_sch_locale;
        if(num_macchine >= 2)
        {
            copia_schelule(M2_sch,M2_sch_locale);
            schedule_locali[1] = M2_sch_locale;
        }
        if(num_macchine == 3)
        {
            copia_schelule(M3_sch,M3_sch_locale);
            schedule_locali[2] = M3_sch_locale;
        }
    }
    for(i = 0;i<dim_job;i++)
    {
        array_job_locale[i].ID = array_job_attuale[i].ID;
        array_job_locale[i].tipo = array_job_attuale[i].tipo;
        array_job_locale[i].proc_time = array_job_attuale[i].proc_time;
        array_job_locale[i].duedate = array_job_attuale[i].duedate;
        array_job_locale[i].deadline = array_job_attuale[i].deadline;
        array_job_locale[i].priority = array_job_attuale[i].priority;
        array_job_locale[i].rel_time = array_job_attuale[i].rel_time;
        array_job_locale[i].adatto = 0;
        array_job_locale[i].schedulato = 0;
    }
    i=0;
    while (i<dim_job)
    {
        temp = aggiungi_job_perm_delta (array_job_locale,dim_job,schedule_locali,Delta);
        k = 0;
        while(k<dim_job)
        {
            if(temp == array_job_locale[k].ID)
            {
                perm1[i].ID =  array_job_locale[k].ID;
                perm1[i].tipo =   array_job_locale[k].tipo;
                perm1[i].proc_time = array_job_locale[k].proc_time;
                perm1[i].duedate = array_job_locale[k].duedate;
                perm1[i].deadline = array_job_locale[k].deadline;
                perm1[i].priority = array_job_locale[k].priority;
                perm1[i].rel_time = array_job_locale[k].rel_time;
                array_job_locale[k].schedulato = 1;
            }
            k++;
        }
        array_job_locale[i].adatto = 0;
        i++;  
    }
   //devo ora liberare lo spazio delle schedule locali
    elimina_schedula(M1_sch_locale);
    if(num_macchine >= 2)
    {
        elimina_schedula(M2_sch_locale);
    }
    if(num_macchine == 3)
    {
        elimina_schedula(M3_sch_locale);
    }
    free(schedule_locali);
    free(array_job_locale);
    return perm1;

}
int aggiungi_job_perm (job1 *array_job_locale,int dim_job,schedula **schedule_locali)
{
	
	tipo_new tempo[3];
	int tempi_ind[3];
	int *disponibilita;
	int disp =0;
        int minimum_alternativo2;
	job *perm;
	job pp;
	int i,k,minimum,tipo,Camp,minimum_alternativo=100000;
	int minimus = 0;	
	elem **indisp;
	int *deadline_vett;
	int *duedate_vett;
	int *else_vett;
	int *set;//insieme dei job possibilmente schedulabili (minimi nella duedate o deadline)	
	int *tipo_job;
	int *tipo_macchine;
	int *campagna_macchine;
	int **setup_matrix;
	int **set_matrix;
	int deadline = -1;
	int duedate = -1;
	int nothing = -1;
	int fine=0;
	int j = 0;
	int jj=0;
	int *setup_vett;
	int st_vt = 0;
	int kk;
	int pos_i = 0;
	int pos_k = 0;
	disponibilita = &disp;
	perm = &pp;
	indisp=malloc(num_macchine*sizeof(elem *));
	deadline_vett=malloc(dim_job*sizeof(int));
	duedate_vett=malloc(dim_job*sizeof(int));
	else_vett=malloc(dim_job*sizeof(int));
	set=malloc(dim_job*sizeof(int));
	tipo_job=malloc(dim_job*sizeof(int));	
	tipo_macchine=malloc(num_macchine*sizeof(int));
	campagna_macchine=malloc(num_macchine*sizeof(int));
	setup_matrix = malloc(dim_job*sizeof(int * ));
	set_matrix = malloc(dim_job*sizeof(int * ));

	for(i=0;i<dim_job;i++)
	{
		setup_matrix[i]=malloc(num_macchine*sizeof(int));
		set_matrix[i]=malloc(num_macchine*sizeof(int));
	}
// 	indisp e' un vettore di puntatori alle liste di indisponibilita' delle macchine
	indisp[0] = M1;
	indisp[1] = M2;
	if(num_macchine == 3)
	{
		indisp[2] = M3;
	}
// ___________________________________________________________________________________
// 	inizializzazione delle strutture dati da utilizzare	
	for(i=0; i<dim_job; i++)
	{
		for(k=0; k<num_macchine; k++)
		{
			setup_matrix[i][k]=-1;
			set_matrix[i][k]=-1;
			if(i == 0)
			{
				tipo_macchine[k] = -1;
				campagna_macchine[k] = -1;
			}
		}
		deadline_vett[i] = -1;
		duedate_vett[i] = -1;
		else_vett[i] = -1;
		set[i] = -1;
		tipo_job[i] = -1;	
	}
// __________________________________________________________________________________

	
	tempo[0] = carica_tempo(schedule_locali[0],M1);//carica gli starting time disponibili senza tenere conto dei possibili setup o della durata dei processing time
	if(num_macchine>=2)
	{
		tempo[1] = carica_tempo(schedule_locali[1],M2);
	}
	if (num_macchine == 3)
	{
		tempo[2] = carica_tempo(schedule_locali[2],M3);
	}
// ___________________________________________________________________________________
// 	ora conosco i tempi minimi di schedulazione sulle singole macchine
// 	a questo punto devo solo cercare il minimo di questi tempi e dato questo stabilire il set di job con release date
// 	anteriore a tale data.

	minimum = tempo[0].fine;
	for (i = 1; i < num_macchine; i++)
	{
		minimum = min(minimum,tempo[i].fine);
	}//ora conosco il minimo 
//_____________________________________________________________________________________

// 	devo costruire il set dei job released
	for(i=0; i<dim_job; i++)// verifico tutti i job uno per uno.
	{
		if
		(
			(array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
			&&
			(array_job_locale[i].rel_time <= minimum)// se puo' essere schedulato perche' gia' rilasciato
		)
		{
			array_job_locale[i].adatto = 1;// lo segno come adatto
			//________________________________________________________________
			if(array_job_locale[i].deadline > 0) // se ha una deadline
			{
				deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline nel vettore delle deadline
			}
			//________________________________________________________________
			
			if(array_job_locale[i].duedate > 0 && array_job_locale[i].deadline == 0)// se ha "solo" una duedate
			{
				duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate nel vettore delle duedate
			}
			//________________________________________________________________

			if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
			{// se non ha ne' duedate ne' deadline
				else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline nel vettore else
			}		
		}
		if
		(
			(array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
			&&
			(array_job_locale[i].rel_time <= minimum_alternativo)
		)//calcolo un minimo alternativo nel caso non esiste nessun job con rel_date inferione al minimo tempo di schedulazione sulle macchine
		{
			minimum_alternativo=array_job_locale[i].rel_time;
		}
	}//ora ho segnato i job che posso schedulare xche' rispettano la release date.

// _______________________________________________________________________________________

// 	cerco ora un elemento con deadline != 0;
	k=-1;
	for(i=0;i<dim_job;i++)
	{
		if(deadline_vett[i] > 0)
		{
			k=i; //cerco il primo elemento con deadline maggiore di 0
			break;	
		}
	}

	//devo cercare il job con il valore + piccolo di deadline
	minimus = 0;
	if(k>=0)// se ho trovato almeno un elemento con valore di deadline maggiore di 0
	{
		minimus = deadline_vett[k];
		for(i=k+1;i<dim_job;i++)
		{
			if
			(
				(deadline_vett[i] != -1)
				&&
				(deadline_vett[i] < deadline_vett[k])
			)
			{
				k=i;// salvo la posizione
				minimus = deadline_vett[k];
			}
		}
		fine=1;// flag che mi avverte che posso terminare
		deadline = 1;// segno che ho trovato una deadline	
	}
	else// se nn ho trovato nessuna deadline cerco le duedate
	{
		k=-1;
		for(i=0;i<dim_job;i++)
		{
			if(duedate_vett[i] > 0)
			{
				k=i;// ho trovato almeno una duedate
				break;	
			}
		}

		minimus = 0;
		if(k>=0)// se ho trovato almeno una duedate
		{
			minimus = duedate_vett[k];
			for(i=k+1;i<dim_job;i++)
			{
				if
					(
					(duedate_vett[i] != -1)
					&&
					(duedate_vett[i] < duedate_vett[k])
				)
				{
					k=i;// salvo la posizione
					minimus = duedate_vett[k];// aggiorno il valore del minimo
				}
			}
			fine=1;//flag che mi dice che posso terminare la ricerca	
			duedate = 1;// ho trovato una duedate
		}
		else //cerco un job privo di duedate e deadline
		{		
			k=-1;
			for(i=0;i<dim_job;i++)
			{
				if(else_vett[i] > 0)
				{
					k=i;// ne ho trovato almeno uno
					break;	
				}
			}
			if(k>=0)//un job qualsiasi va bene
			{
				fine = 1; 
				nothing = 1;
			}
			
		}
	}

	if(fine==0)//non posso aggiungere job perche' nessun job ha release date inferiore o uguale al minimo tempo disponibile sulle macchine.
	{
//  	per ogni macchina verifico che minimum non si trovi in un periodo di
// 	indisponibilita', nel caso fosse inserisco in tempi_ind il valore 
// 	dell'estremo superiore di tale periodo	
		for(i=0;i<num_macchine;i++)
		{
			tempi_ind[i]=trova_edge_indisp(indisp[i],minimum_alternativo);
		}
//_____________________________________________________________________________________
// 		calcolo adesso il minimo di questi nuovi tempi		
		minimum_alternativo2=tempi_ind[0];
		for (i = 1; i < num_macchine; i++)
		{
			if(tempi_ind[i]<minimum_alternativo2)
			{
				minimum_alternativo2=tempi_ind[i];
			}
			
		}
// _____________________________________________________________________________________

                   i=0;
                   for(i=0;i<num_macchine;i++)
                   {
                      tempo[i].fine=tempi_ind[i];
                   }
                   minimum=minimum_alternativo2;
                
//                 minimum_alternativo=max(minimum_alternativo,minimum_alternativo2);
                for(i=0; i<dim_job; i++)
                {
                   if
                   (
                    (array_job_locale[i].schedulato == 0)//aggiungo solo job non ancora schedulati al set possibile
                    &&
                    (array_job_locale[i].rel_time <= minimum)
                   )
                   {
                      array_job_locale[i].adatto = 1;
// 			_______________________________________________________________
                      if(array_job_locale[i].deadline > 0)
                      {
                         deadline_vett[i] = array_job_locale[i].deadline;//salvo le deadline
                      }

// 			_______________________________________________________________
                      if(array_job_locale[i].duedate > 0 && array_job_locale[i].deadline == 0)
                      {
                         duedate_vett[i] = array_job_locale[i].duedate;//salvo le duedate
                      }
// 			_________________________________________________________________
                      if(array_job_locale[i].duedate == 0 && array_job_locale[i].deadline == 0)
                      {
                         else_vett[i] = 1;//salvo la posizione dei job privi di duedate e deadline
                      }		
                   }
                }

// come sopra devo individuare il job che deadline o duedate minima
                k=-1;
                for(i=0;i<dim_job;i++)
                {
                   if(deadline_vett[i] > 0)
                   {
                      k=i;
                      break;	
                   }
                }
                minimus = 0;
                if(k>=0)
                {
                   minimus = deadline_vett[k];
// 	ho trovato almeno un elemento che ha deadline > 0
                   for(i=k+1;i<dim_job;i++)
                   {
                      if
                      (
                       (deadline_vett[i] != -1)
                       &&
                       (deadline_vett[i] < deadline_vett[k])
                      )
                      {
                         k=i;// salvo la posizione
                         minimus = deadline_vett[k];
                      }
                   }
                   fine=1;
                   deadline = 1;// segno che ho trovato una deadline	
                }
                else// se nn ho trovato nessuna deadline cerco le duedate
                {
                   k=-1;
                   for(i=0;i<dim_job;i++)
                   {
                      if(duedate_vett[i] > 0)
                      {
                         k=i;
                         break;	
                      }
                   }
                   minimus = 0;
                   if(k>=0)
                   {
                      minimus = duedate_vett[k];
// 	ho trovato almeno un elemento che ha duedate > 0 && deadline == 0
                      for(i=k+1;i<dim_job;i++)
                      {
                         if
                         (
                          (duedate_vett[i] != -1)
                          &&
                          (duedate_vett[i] < duedate_vett[k])
                         )
                         {
                            k=i;// salvo la posizione
                            minimus = duedate_vett[k];
                         }
                      }
                      fine=1;	
                      duedate = 1;
                   }
                   else //cerco un job privo di duedate e deadline
                   {		
                      k=-1;
                      for(i=0;i<dim_job;i++)
                      {
                         if(else_vett[i] > 0)
                         {
                            k=i;
                            break;	
                         }
                      } 
                      if(k>=0)
                      {
                         fine = 1; 
                         nothing = 1;
                      }
			
                   }
                }
//                 fine adesso dovrebbe essere uguale ad 1
	}		
	if(fine==1)// dovrebbe essere sempre vera questa condizione a questo punto
	{
		if (deadline == 1)
		{//minimus indica il valore + basso della deadline
			i=0;
// 			salvo tutti i job che hanno deadline pari a minimus
			while(i<dim_job)
			{
				if(deadline_vett[i]==minimus)
				{
					set[i] = array_job_locale[i].ID;
					tipo_job[i] = array_job_locale[i].tipo;
					
				}
				i++;
			}
			
		}
		else if (duedate == 1)
		{
			//minimus indica il valore + basso della duedate
// 			salvo i job con i valori minimi di duedate
			i=0;
			while(i<dim_job)
			{
				if(duedate_vett[i]==minimus)
				{
					set[i] = array_job_locale[i].ID;
					tipo_job[i] = array_job_locale[i].tipo;
					
				}
				i++;
			}			
		} 
		else if (nothing == 1)
		{
// 			salvo tutti i job che non hanno ne' duedate ne' deadline
			i=0;
			while(i<dim_job)
			{
				if(else_vett[i]!=-1)
				{
					set[i] = array_job_locale[i].ID;
					tipo_job[i] = array_job_locale[i].tipo;
					
				}
				i++;
			}
		}
// _________________________________________________________________________________
// 		per ogni macchina salvo le informazioni sul tipo di job e sullo
// 		stado della campagna
		i=0;
		while(i<num_macchine)
		{
			if(tempo[i].fine == minimum)
			{
				tipo_macchine[i] = tempo[i].tipo;
				campagna_macchine[i] = tempo[i].Camp;
			}
			i++;
		}
//__________________________________________________________________________________
		
		for(i=0;i<dim_job;i++)
		{	
			for(k=0;k<num_macchine;k++)
			{
				if
				(
					(set[i]!=-1)// se il job e' elegibile
					&&
					(tipo_macchine[k]!=-1)// se la macchina non e' vuota
				)
				{
					if(tipo_job[i] == tipo_macchine[k])// se il job e' dello stesso tipo del precedente
					{
						j = 0;
						for(j=0;j<num_tipi;j++)
						{
							if(array_tipi[j].ID==tipo_macchine[k])// cerco la campagna massima
							{
								if(campagna_macchine[k]<array_tipi[j].MaxOpCamp)
								{
	// 								eureka! posso shedulare questo job immediatamente
	// 								la macchina k-esima verra'assegnata al job
									jj=0;
									st_vt = 0;
									setup_vett =&st_vt;
									while(jj<num_job)
									{
										if(array_job[jj].ID == set[i])
										{
											perm[0]=array_job[jj];
											break;
										}
									jj++;
									}
									verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
									aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
									kk = set[i];
									free(indisp);
									free(deadline_vett);
									free(duedate_vett);
									free(else_vett);
									free(set);
									free(tipo_job);	
									free(tipo_macchine);
									free(campagna_macchine);
								for(i=0;i<dim_job;i++)
								{
									free(setup_matrix[i]);
									free(set_matrix[i]);
	
								}
								free(setup_matrix);
								free(set_matrix);	
								return kk;
								}
								else
								{
								setup_matrix[i][k]= Cmaj_matrix[campagna_macchine[k]-1][tipo_job[i]-1];
								set_matrix[i][k] = set[i];
								}	
							}
						}	
					}
					else
					{
						if (tipo_macchine[k] == 0)//vuol dire che nn ci sono altri job gia' schedulati
						{
	// 					nn devo pagare setup
							jj=0;
							st_vt = 0;
							setup_vett =&st_vt;
							while(jj<num_job)
							{
								if(array_job[jj].ID == set[i])
								{
									perm[0]=array_job[jj];
									break;
								}
							jj++;
							}
							verifica_macchina(schedule_locali[k],indisp[k],disponibilita,setup_vett,0,perm,0);
							aggiungi_schedula(schedule_locali[k],perm[0],disponibilita[0],setup_vett[0]);
							kk = set[i];
							free(indisp);
							free(deadline_vett);
							free(duedate_vett);
							free(else_vett);
							free(set);
							free(tipo_job);	
							free(tipo_macchine);
							free(campagna_macchine);
							for(i=0;i<dim_job;i++)
							{
								free(setup_matrix[i]);
								free(set_matrix[i]);
							}
							free(setup_matrix);
							free(set_matrix);
							return kk;
						}
						else
						{
							setup_matrix[i][k]= Cmaj_matrix[campagna_macchine[k]-1][tipo_job[i]-1];
							set_matrix[i][k] = set[i];
						}
					}
				}
			}
		}	
	}
        else
        {
           printf("\n Error\n\a");
           exit(0);
        }	
// 	 a questo punto ho la matrice dei setup e devo scegliere quello a costo minimo.
	pos_i = 0;
	pos_k = 0;
	for(i=0;i<dim_job;i++)
	{
		for(k=0;k<num_macchine;k++)
		{
// 			devo trovare un elemento nn nullo
			if
			(
				(setup_matrix[pos_i][pos_k] == -1)
				&&
				(setup_matrix[i][k]!=-1)
			)
			{
				pos_i = i;
				pos_k = k;
			}
			if
			(
				(setup_matrix[i][k]>=0)
				&&
				(setup_matrix[i][k] < setup_matrix[pos_i][pos_k])
			)
			{
				pos_i = i;
				pos_k = k;
			}
		}
	}
	j=0;
	
	st_vt = 1;
	setup_vett =&st_vt;
	while(j<num_job)
	{
		if(array_job[j].ID == set_matrix[pos_i][pos_k])
		{
			perm[0]=array_job[j];
			break;
		}
	j++;
	}
	verifica_macchina(schedule_locali[pos_k],indisp[pos_k],disponibilita,setup_vett,0,perm,0);
	aggiungi_schedula(schedule_locali[pos_k],perm[0],disponibilita[0],setup_vett[0]); 
	kk = set_matrix[pos_i][pos_k];
	free(indisp);
	free(deadline_vett);
	free(duedate_vett);
	free(else_vett);
	free(set);
	free(tipo_job);	
	free(tipo_macchine);
	free(campagna_macchine);
	for(i=0;i<dim_job;i++)
	{
		free(setup_matrix[i]);
		free(set_matrix[i]);
	}
	free(setup_matrix);
	free(set_matrix);
	return kk;
	
}
void inizializza_struttura_euristiche(int tipo_eur) 
{
    switch(tipo_eur) 
    {
        case 0:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_EDD_1_tipo;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 1:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_EDD_2_tipo;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 2:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_base;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 3:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_SPT_semplice;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 4:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_LLF;
            funzioni[0].ID_heur = 0; 
            break;
        }
        case 5:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_delta_7ore;
            funzioni[0].ID_heur = 0; 
            break;
        }
        case 6:
        {
	    funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_delta_proc_medio;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 7:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_delta_mezzo_proc_medio;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 8:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_LLF_delta_7ore;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 9:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_LLF_delta_proc_medio;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 10:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_delta_24ore;
            funzioni[0].ID_heur = 0;
	    break;
        }
        case 11:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_delta_3_proc_medio;
	    funzioni[0].ID_heur = 0;
	    break;	
        }
        case 12:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_LLF_delta_24ore;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 13:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_LLF_delta_3_proc_medio;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 14:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_LLF_delta_mezzo_proc_medio;
            funzioni[0].ID_heur = 0;
            break;
        }
        case 15:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_delta_proc_medio;
            funzioni[0].ID_heur = 0;
            funzioni[1].perc_utilizzo = 0;
            funzioni[1].funz = permutazione_base;
            funzioni[1].ID_heur = 1;
            break;
        }
        case 16:
        {
            	funzioni[0].perc_utilizzo = 0;
		funzioni[0].funz = permutazione_EDD_1_tipo;
		funzioni[0].ID_heur = 0;
		funzioni[1].perc_utilizzo = 0;
		funzioni[1].funz = permutazione_EDD_2_tipo;
		funzioni[1].ID_heur = 0;
		funzioni[2].perc_utilizzo = 0;
		funzioni[2].funz = permutazione_base;
		funzioni[2].ID_heur = 0;
		funzioni[3].perc_utilizzo = 0;
		funzioni[3].funz = permutazione_SPT_semplice;
		funzioni[3].ID_heur = 0;
		funzioni[4].perc_utilizzo = 0;
		funzioni[4].funz = permutazione_LLF;
		funzioni[4].ID_heur = 0; 
		funzioni[5].perc_utilizzo = 0;
		funzioni[5].funz = permutazione_delta_7ore;
		funzioni[5].ID_heur = 0; 
		funzioni[6].perc_utilizzo = 0;
		funzioni[6].funz = permutazione_delta_proc_medio;
		funzioni[6].ID_heur = 0;
		funzioni[7].perc_utilizzo = 0;
		funzioni[7].funz = permutazione_delta_mezzo_proc_medio;
		funzioni[7].ID_heur = 0;
		funzioni[8].perc_utilizzo = 0;
		funzioni[8].funz = permutazione_LLF_delta_7ore;
		funzioni[8].ID_heur = 0;
		funzioni[9].perc_utilizzo = 0;
		funzioni[9].funz = permutazione_LLF_delta_proc_medio;
		funzioni[9].ID_heur = 0;
		break;
        }
        case 17:
        {
            funzioni[0].perc_utilizzo = 0;
            funzioni[0].funz = permutazione_EDD_1_tipo;
            funzioni[0].ID_heur = 0;

            funzioni[1].perc_utilizzo = 0;
            funzioni[1].funz = permutazione_EDD_2_tipo;
            funzioni[1].ID_heur = 1;

            funzioni[2].perc_utilizzo = 0;
            funzioni[2].funz = permutazione_base;
            funzioni[2].ID_heur = 2;

            funzioni[3].perc_utilizzo = 0;
            funzioni[3].funz = permutazione_SPT_semplice;
            funzioni[3].ID_heur = 3;

            funzioni[4].perc_utilizzo = 0;
            funzioni[4].funz = permutazione_LLF;
            funzioni[4].ID_heur = 4;

            funzioni[5].perc_utilizzo = 0;
            funzioni[5].funz = permutazione_delta_7ore;
            funzioni[5].ID_heur = 5;

            funzioni[6].perc_utilizzo = 0;
            funzioni[6].funz = permutazione_delta_proc_medio;
            funzioni[6].ID_heur = 6;

            funzioni[7].perc_utilizzo = 0;
            funzioni[7].funz = permutazione_delta_mezzo_proc_medio;
            funzioni[7].ID_heur = 7;

            funzioni[8].perc_utilizzo = 0;
            funzioni[8].funz = permutazione_LLF_delta_7ore;
            funzioni[8].ID_heur = 8;

            funzioni[9].perc_utilizzo = 0;
            funzioni[9].funz = permutazione_LLF_delta_proc_medio;
            funzioni[9].ID_heur = 9;

            funzioni[10].perc_utilizzo = 0;
            funzioni[10].funz = permutazione_delta_24ore;
            funzioni[10].ID_heur = 10;

            funzioni[11].perc_utilizzo = 0;
            funzioni[11].funz = permutazione_delta_3_proc_medio;
            funzioni[11].ID_heur = 11;

            funzioni[12].perc_utilizzo = 0;
            funzioni[12].funz = permutazione_LLF_delta_24ore;
            funzioni[12].ID_heur = 12;

            funzioni[13].perc_utilizzo = 0;
            funzioni[13].funz = permutazione_LLF_delta_3_proc_medio;
            funzioni[13].ID_heur = 13;

            funzioni[14].perc_utilizzo = 0;
            funzioni[14].funz = permutazione_LLF_delta_mezzo_proc_medio;
            funzioni[14].ID_heur = 14;
            //funzioni[15].perc_utilizzo = 0;
            //funzioni[15].funz = permutazione_fittizia;
            //funzioni[15].ID_heur = 15;
            break;
        }
    }
}



