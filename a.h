for (i = 0; i < qntd; i++)
	{
		for (j = 0; j < qntd; j++)
		{
			if (vet[j].tempo_ranking < vet[i].tempo_ranking)
			{
				int tmp = vet[i].tempo_ranking;
				vet[i].tempo_ranking = vet[j].tempo_ranking;            
				vet[j].tempo_ranking = tmp;
				char nome_tmp[20];
				strcpy(nome_tmp, vet[i].nome_ranking);
				strcpy(vet[i].nome_ranking, vet[j].nome_ranking);
				strcpy(vet[j].nome_ranking, nome_tmp);                    
			}
		}
	}