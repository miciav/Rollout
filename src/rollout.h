
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifndef ROLLOUTH
#define ROLLOUTH
#define Max_tipi 40 
// secondi prima che la funzione clock cominci a tornare indietro
#define MAX_SECONDS 2147

struct elemento
	{
	int inizio;
	int fine;
	struct elemento *next;
	};
typedef struct elemento elem;

struct sch //questa struttura conterr�le schedulazioni per ogni macchina
	{
	int ID_job;
	int tipo;
	int inizio;
	int fine;
	int Lmax;
	int Tardy;
	int index_camp;
	struct sch *next;
	};
typedef struct sch schedula;


struct jb
	{
	int ID;
	int tipo;
	int proc_time;
	int duedate;
	int deadline;
	int priority;
	int rel_time;
	};
typedef struct jb job;

struct jb1
	{
	int ID;
	int tipo;
	int proc_time;
	int duedate;
	int deadline;
	int priority;
	int rel_time;
	int adatto;
	int schedulato;
	};
typedef struct jb1 job1;

struct TIPO
	{
	int ID;
	int MaxOpCamp;	
	};
typedef struct TIPO tipo;
struct TIPO_new
	{
	int fine;
	int tipo;
	int Camp;		
	};
typedef struct TIPO_new tipo_new;

struct inf 
	{
	int ID_job;
	int tipo;
	int proc_time;
	int duedate;
	int deadline;
	int priority;
	int rel_time;
	int index_camp;
	int ID_heur;
	int macchina;
	int inizio;
	int fine;
	int Lmax_pers;
	int Tardy_pers;
	int Lmax;
	int Cmax;
	int Tardy;
	int feasible;
	struct inf *next;
	};
typedef struct inf next_elem;
struct TIPO_funz
	{
	int perc_utilizzo;
	int ID_heur;
	job * (*funz)(job *,int);	
	};
typedef struct TIPO_funz strutt_funz;

struct TIPO_quaterna
	{
	int Lmax;
	int Cmax;
	int Tardy;
	int Feasible;		
	};
typedef struct TIPO_quaterna quaterna;

int num_job = 0;
int num_heur ;
elem *M1,*M2,*M3;
job *array_job;
job* permutazione_finale;
job* permutazione_buffer;
tipo *array_tipi;
schedula *M1_sch,*M2_sch,*M3_sch;
job *best_perm;
int num_macchine = 0;
int num_tipi  = 0;
int Cmaj_matrix[Max_tipi][Max_tipi];
//_______________________________________________________________________________
int Lmax = 0;// massima lateness
int Cmax = 0;//Makespan
int Tardy = 0;//numero di tardy jobs per schedula
int Feasible = 1; // mi dice se la schedula costruita �feasible
//_________________________________________________________________k______________
int Lmax_best=-65000;
int Cmax_best=-65000;
int Tardy_best=-65000;
int Feasible_best=-2;
int swap_lat_tard=0;

strutt_funz *funzioni;
int politica_pruning; //indica quale politica di esclusione si sta utilizzando
int num_heur_utilizz; //mantiene il numero di euristiche che si stanno utilizzando



time_t tempo_secondi_inizio1 = 0 ;
clock_t tempo_inizio1=0,tempo_inizio2 = 0,tempo_inizio3 = 0;
time_t tempo_secondi_fine1 = 0;
clock_t tempo_fine1=0,tempo_fine2 = 0,tempo_fine3 = 0;



int num_Heur ;


//___MIE FUNZIONI E VARIABILI____
int euristica; //indica quale delle euristiche bisogna lanciare nel caso non si usi il rollout
int local_search_mode;

int Tempo;
int rollout_or_heuristic;
clock_t tempo_inizio4 = 0;
clock_t tempo_fine4 = 0;

char instance_file[256];
char output_file[256]; // Nome file di output della configurazione
int force;
int tipo_eur,tipo_rollout;


FILE *f_out_Lmax, *f_out_Cmax, *f_out_Tardy, *f_out_Feas, *f_out_Tempo;
char *out_Lmax, *out_Cmax, *out_Tardy, *out_Feas, *out_Tempo;
char str_temp[100];


void escludi_euristica(strutt_funz *funzioni, int politica, int cont_livelli);
float *calcola_statistiche();
void carica_file(char *path_file_conf);
void politica_never_win(strutt_funz *funzioni);
void politica_less3perc(strutt_funz *funzioni);
void politica_less5perc(strutt_funz *funzioni);
void politica_less7perc(strutt_funz *funzioni);
void politica_less3perc_reset(strutt_funz *funzioni);
void politica_less5perc_reset(strutt_funz *funzioni);
void politica_less7perc_reset(strutt_funz *funzioni);
void politica_linear_spread(strutt_funz *funzioni, int cont_livelli);
void politica_linear_spread_reset(strutt_funz *funzioni, int cont_livelli);
void politica_never_win_one_at_a_time(strutt_funz *funzioni);


void swap(schedula *M1_sch, schedula *M2_sch, elem *M1, elem *M2, schedula *sch_swap1, schedula *sch_swap2, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2);
void insert(schedula *InCuiInserire, schedula *InCuiLevare, elem *M1, elem *M2, schedula *sch_insert, schedula *sch_following, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2);
void ordina_candidati(schedula **arraySch, elem **arrayM);
int ricerca_locale_tra_macchine(schedula *M1_sch, schedula *M2_sch, schedula *M3_sch);
int ricerca_locale_tra_macchine_VND(schedula *M1_sch, schedula *M2_sch, schedula *M3_sch);
int ricerca_locale_tra_macchine_coda(schedula *M1_sch, schedula *M2_sch, schedula *M3_sch);
void sostituisci_schedule(schedula **arraySch, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2, schedula *schedula_di_lavoro3);
int VNS_ricerca_locale(schedula *M1_sch_buffer,schedula *M2_sch_buffer,schedula *M3_sch_buffer, elem *M1, elem *M2, elem *M3);
void sposta_in_coda(schedula *M_sch, elem *M, schedula *in_coda, schedula *sch_coda);
void insert_coda(schedula *InCuiInserire, schedula *InCuiLevare, elem *M1, elem *M2, schedula *sch_insert, schedula *schedula_di_lavoro1, schedula *schedula_di_lavoro2);
//______________________________________________________________________________

int carica_indisponibilita(FILE *istanza);
void distruggi_indisponibilita(int num_macchine);
void elimina_lista(elem *punt_lista);
void elimina_schedula(schedula *punt_schedula);
int carica_lista_job(FILE * istanza);
int costruisci_e_valuta_schedula(schedula *M1_sch_locale,schedula *M2_sch_locale,schedula *M3_sch_locale,next_elem *prossimo,job *perm,int dim_job);
void aggiungi_schedula(schedula *M_sch, job task,int time,int setup_vet);
void verifica_macchina(schedula *temp, elem  *temp1, int *disponibilita, int *setup_vett,int p,job *perm,int i);
job *permutazione_EDD_1_tipo(job *array_job_attuale, int dim_job);	
job *permutazione_EDD_2_tipo(job *array_job_attuale, int dim_job);
job *permutazione_base(job *array_job_attuale, int dim_job);

job *permutazione_delta_2(job *array_job_attuale, int dim_job);
job *permutazione_delta_3(job *array_job_attuale, int dim_job);
job *permutazione_delta_5(job *array_job_attuale, int dim_job);
job *permutazione_delta_10(job *array_job_attuale, int dim_job);
job *permutazione_delta_15(job *array_job_attuale, int dim_job);
job *permutazione_delta_20(job *array_job_attuale, int dim_job);
job *permutazione_delta_25(job *array_job_attuale, int dim_job);
job *permutazione_delta_30(job *array_job_attuale, int dim_job);
job *permutazione_delta_35(job *array_job_attuale, int dim_job);
job *permutazione_delta_40(job *array_job_attuale, int dim_job);
job *permutazione_delta_50(job *array_job_attuale, int dim_job);

job *permutazione_SPT_semplice(job *array_job_attuale, int dim_job);
job *permutazione_fittizia(job *array_job_attuale, int dim_job);

job *permutazione_LLF_delta_10(job *array_job_attuale, int dim_job);
job *permutazione_LLF_delta_5(job *array_job_attuale, int dim_job);
job *permutazione_LLF_delta_2(job *array_job_attuale, int dim_job);

job *permutazione_LLF(job *array_job_attuale, int dim_job); 
//_______________________________________________________________________________
job *permutazione_delta_7ore(job *array_job_attuale, int dim_job);
job *permutazione_delta_24ore(job *array_job_attuale,int dim_job);
job *permutazione_delta_3_proc_medio(job *array_job_attuale,int dim_job);
job *permutazione_delta_proc_medio(job *array_job_attuale, int dim_job);
job *permutazione_delta_mezzo_proc_medio(job *array_job_attuale, int dim_job);
//_______________________________________________________________________________

job *permutazione_LLF_delta_7ore(job *array_job_attuale, int dim_job);
job *permutazione_LLF_delta_24ore(job *array_job_attuale,int dim_job);
job *permutazione_LLF_delta_3_proc_medio(job *array_job_attuale,int dim_job);
job *permutazione_LLF_delta_proc_medio(job *array_job_attuale, int dim_job);
job *permutazione_LLF_delta_mezzo_proc_medio(job *array_job_attuale, int dim_job);
//_______________________________________________________________________________


void valuta_schedula(schedula *M1_sch_locale,schedula *M2_sch_locale,schedula *M3_sch_locale,next_elem *prossimo);
void azzera_schedule(void);
void inizializza_struttura_euristiche(int tipo_eur);

job *rollout(int force,char *instance_file);
job *rollout_old(int force,char *instance_file);
job *rollout_modificato1(int force,char *instance_file);
job *rollout_modificato2(int force,char *instance_file);
job *rollout_modificato3(int force,char *instance_file);
job *rollout_modificato4(int force,char *instance_file);
job *rollout_modificato5(int force,char *instance_file);
job *rollout_modificato6(int force,char *instance_file);
job *rollout_antonio(int force,char *instance_file);
job *rollout_time(int force,char *instance_file,int Tempo);
void stampa_risultati_a_video(int flag);
void stampa_risultati_su_file(FILE *f_out,char *instance_file,int flag,int force);
tipo_new carica_tempo(schedula *M_sch, elem *M);
int min(int a, int b);
int max(int a, int b);
int aggiungi_job_perm (job1 *array_job_locale,int dim_job,schedula **schedule_locali);
int aggiungi_job_perm_delta (job1 *array_job_locale,int dim_job,schedula **schedule_locali,int delta);
void copia_schelule(schedula *M1_sch,schedula *M1_sch_locale);
void distruggi_schedule(int num_macchine); 
schedula *mossa(schedula *M_sch, elem *M, int pos_iniziale, int pos_finale);
int VNS(schedula *M1_sch_buffer,schedula *M2_sch_buffer,schedula *M3_sch_buffer);
int VNS_per_macchina(schedula *M_sch,elem *M);
quaterna *valuta_singola_schedula(schedula *M_sch);
int bilanciamento_schedule(schedula *M1_sch_locale,schedula *M2_sch_locale,schedula *M3_sch_locale);
void stampa_percentuali_utilizzo_a_video(void);
int seleziona_prossimo_job(next_elem *lista_prossimi_vincitori,int cont_livelli);
void stampa_permutazioni(job *permutazione,int dim);
int trova_posizione_assoluta(job *array,int pos_rel);
void verifica_cambiamento_macchina(int *primo_passo_M1,int *primo_passo_M2,int *primo_passo_M3);
void salva_se_meglio(job* permutazioni,job *best_perm,int cont_livelli,int iter_for);
void inizializza_perutazione_migliore(job *perm_di_passaggio); 
int trova_edge_indisp(elem *M,int minimum);
void stampa_sequenza_macchina(schedula *M);
void verifica_se_elementi_uguali(job *perm_di_passaggio,int num_job);
int calcola_numero_eur(int tipo_eur);
int calcola_proc_time_medio(void);
int *trova_migliori(next_elem *lista_prossimi_vincitori,int cont_livelli,int num_next_job);
int *trova_prossimi_migliori(next_elem *lista_prossimi_vincitori,int cont_livelli,int num_next_job);
int *purifica_lista_job_prescelti(int *lista_migliori_passo_precedente,int dim_lista, int *prossima_dimensione);
int CmaxSchedula(schedula *M_sch);
int MaxCmaxSchedula(schedula **vett_schedule,int num_macchine);
int MinCmaxSchedula(schedula **vett_schedule,int num_macchine);
void AggiornaTempProx(int k,job perm,schedula *M_sch,next_elem *temp_prox);
#endif
