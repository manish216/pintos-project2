#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "process.h"
#include "filesys/directory.h"

#include "pagedir.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);
struct file * fileSearch(int fd);
extern bool running;
//bool fileNameSearch(char filename);

struct process_files{
	struct file *fileptr;
	int fd;
	struct list_elem elem;
};

bool valid (void * vaddr);
void kill (void);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int * p = f->esp;

  
  int system_call = * p;
    
	switch (system_call)
	{
		case SYS_HALT:
		shutdown_power_off();
		break;

		case SYS_EXIT:
		thread_current()->exitcode=*(p+1);
		thread_current()->parent->ex = true;
		
		thread_exit();
		break;

		case SYS_WRITE://0x10123420;
		/*if(*(p+1)<0xc0000000 && *(p+1)>0x08084000){printf("hiiiiiiiiiii");
		thread_current()->exitcode=-1;
		thread_current()->parent->ex = true;
		thread_exit();}
			else
			{*/
				//printf("chalooooo");
			
		if((*(p+1)==1))
		{
			
			
			//0xc0000000
			putbuf(*(p+2),*(p+3));
			//printf("\nbuffer===== :%p",  *(p+2));
			//printf("\nbuffer1===== :%X",  *(p+2));
			//printf("\nsize====: %p",*(p+3));
		}
		//if(*(p+1)!=NULL)
		else if(*(p+1)==0){thread_current()->exitcode=-1;
		thread_current()->parent->ex = true;
		thread_exit();}
		else{
		//struct file *fptr=fileSearch(*(p+1));
		//printf("hello");
		f->eax=file_write(fileSearch(*(p+1)),*(p+2),*(p+3));

		}//}
		break;

        case SYS_CREATE:
		if(*(p+1)!=NULL)
		{	//char *filename=*(p+1);
			//bool fileExists=fileNameSearch(p+1);
			if(filesys_open(*(p+1))!=NULL){//printf("exjsjjsjs");
			f->eax=0;}
			else{struct file *fileToBeClosed=fileSearch(*(p+1));
		if(fileToBeClosed!=NULL){
		file_close(fileToBeClosed);
		fileRemove(fileToBeClosed);}
				int size= strlen(*(p+1));
				//printf("sizeeeeeeeeeeeeeeee%d",size);
				if(size<15){
				f->eax=filesys_create(*(p+1),*(p+2));}
				else if(size>14)
				{f->eax=0;}
				}
		}
		
		else{if(*(p+1)==NULL){
			//f->eax=0;
		thread_current()->parent->ex = true;
		thread_current()->exitcode=-1;
		thread_exit();
		}}
		
		break;
                
        case SYS_OPEN:
        //char name = (char)(p+1);
		//printf("%shiiiiii",*(p+1));
		if(*(p+1)==NULL){
        return -1;
        }
		struct file *fx = filesys_open(*(p+1));
		if(fx==NULL){
			f->eax=-1;}
		else{
			struct process_files *fp=malloc(sizeof(struct process_files));
			fp->fileptr=fx;
			thread_current()->fd++;
			fp->fd=thread_current()->fd;
			list_push_back(&thread_current()->files,&fp->elem);
			f->eax=thread_current()->fd;
		}
		
	
		break;

		case SYS_CLOSE:
		if(*(p+1)!=NULL){
		struct file *fileToBeClosed=fileSearch(*(p+1));
		if(fileToBeClosed!=NULL){
		file_close(fileToBeClosed);
		fileRemove(fileToBeClosed);}}
		break;

		default:
		printf("No match\n");
		case SYS_FILESIZE:
		if(*(p+1)!=NULL){
		struct file *requiredFile=fileSearch(*(p+1));
		if(requiredFile!=NULL){
			//struct inode *in=file_get_inode(requiredFile);
			
			int length= file_length(requiredFile);
			f->eax=length;
		}
		else{
			f->eax=-1;
		}
		}
		break;
		case SYS_READ: 
		//printf("hhhhhhhhhh");
		if(*(p+1)!=NULL){struct file *fileToBeRead=fileSearch(*(p+1));
		if(fileToBeRead!=NULL){
			if(*(p+2)<0xc0000000){
			//printf("%dhhhhhhhhhhhhh",file_length(fileToBeRead));
		int butes=file_read(fileToBeRead,*(p+2),*(p+3));
		f->eax=butes;}
		else{thread_current()->exitcode= -1;
      thread_current()->parent->ex=true;
      thread_exit();}
		}}
		break;
		
		case SYS_EXEC:
		if(*(p+1)!=NULL){
		int pid_child=process_execute(*(p+1));
		//printf("pid_chilllllld=%d",pid_child);
		return pid_child;
		}
	}
}



bool valid(void * vaddr)
{
  return (is_user_vaddr(vaddr) && 
    pagedir_get_page(thread_current()->pagedir,vaddr)!=NULL);
}

void fileRemove(struct file * fileToBeRemoved){
	struct list_elem *e;
	//printf("fd===%d",fd);
	for(e=list_begin(&thread_current()->files);e!=list_end(&thread_current()->files);
	e=list_next(&thread_current()->files)){
		struct process_files *procFiles=list_entry(e,struct process_files,elem);
		//printf("files=%d",procFiles->fd);
		if(procFiles->fileptr==fileToBeRemoved){
			//printf("foundddddd%d",fd);
			list_remove(e);
			//printf("suxeess");
		}
	}

	
}
struct file* fileSearch(int fd){
	struct list_elem *e;
	//printf("fd===%d",fd);
	for(e=list_begin(&thread_current()->files);e!=list_end(&thread_current()->files);
	e=list_next(&thread_current()->files))
	{	//printf("size=%d",list_size(&thread_current()->files));
		struct process_files *procFiles=list_entry(e,struct process_files,elem);
		//printf("files=%d",procFiles->fd);
		if(procFiles->fd==fd){
			//printf("foundddddd%d",fd);
			return procFiles->fileptr;
		}
		
	}
	return NULL;
}