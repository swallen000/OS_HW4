#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>

using namespace std;

typedef struct type
{
	int pagenumber;
	int framenumber;
	int priority;
}TLB;

int main(int argc , char *argv[]){
	int n;
	FILE *address,*memory,*result;
	memory=fopen(argv[1],"rb");
	address=fopen(argv[2],"rb");
	result=fopen("results.txt","wb+");
	fscanf(address , "%d" , &n);
	int input[n];
	TLB tlb[16];
	int page_table[256];
	int frame_index=0;
	int hit_time=0;
	int page_fault=0;
	for(int i=0;i<256;i++){
		page_table[i]=-1;
	}
	for(int i=0;i<16;i++){
		tlb[i].pagenumber=-1;
		tlb[i].framenumber=-1;
	}
	for(int i=0;i<n;i++){
		fscanf(address , "%d" , &input[i]);
	}
	for(int i=0;i<n;i++){
		int page_number;
		int offset;
		int j;
		bool tlb_hit=false;
		bool tlb_full=false;
		bool pagefault=false;
		int8_t buff;
		
		page_number=input[i]/((int)pow(2,8));
		offset=input[i]%((int)pow(2,8));
		//cout<<"page number is : "<<page_number<<" offset : "<<offset<<endl;
		//cout<<"now input is : "<<input[i]<<endl;
		fseek(memory,page_number*(int)pow(2,8)+offset,SEEK_SET);
		fread(&buff,sizeof(int8_t),1,memory);
		//printf("%x\n",buff);
		for(j=0;j<16;j++){
			if(tlb[j].pagenumber==page_number){
				hit_time++;
				tlb_hit=true;
				break;
			}
		}
		if(tlb_hit){
			int frame=j;
			tlb[j].priority=i;
			//cout<<tlb[j].framenumber*256+offset<<" ";
			//cout<<static_cast<int>(buff)<<endl;	
			fprintf(result,"%d %d\n",tlb[j].framenumber*256+offset,static_cast<int>(buff));
		}
		else{
			if(page_table[page_number]==-1){
				pagefault=true;
				page_fault++;
				page_table[page_number]=frame_index;
				//cout<<"now frame number is : "<<frame_index<<endl;
			}
			for(j=0;j<16;j++){
				if(tlb[j].pagenumber==-1)
					break;
			}
			if(j==16){
				//cout<<"tlb is full!\n";
				int min=1e9;
				int digit;
				for(int k=0;k<16;k++){
					if(min>tlb[k].priority){
						min=tlb[k].priority;
						digit=k;
					}
				}
				tlb[digit].pagenumber=page_number;
				tlb[digit].framenumber=page_table[page_number];
				tlb[digit].priority=i;
				//cout<<tlb[digit].framenumber*256+offset<<" ";
				//cout<<static_cast<int>(buff)<<endl;	
				fprintf(result,"%d %d\n",tlb[digit].framenumber*256+offset,static_cast<int>(buff));
			}
			else{
				tlb[j].pagenumber=page_number;
				tlb[j].framenumber=page_table[page_number];
				tlb[j].priority=i;
				//cout<<tlb[j].framenumber*256+offset<<" ";
				//cout<<static_cast<int>(buff)<<endl;	
				fprintf(result,"%d %d\n",tlb[j].framenumber*256+offset,static_cast<int>(buff));
			}
			if(pagefault)
				frame_index++;
		}
	}
	fprintf(result,"TLB hits: %d\n",hit_time);
	fprintf(result,"Page Faults: %d\n",page_fault);
	//cout<<"hit : "<<hit_time<<endl;
	//cout<<"fault : "<<page_fault<<endl;
	fclose(memory);
	fclose(address);
	return 0;
}