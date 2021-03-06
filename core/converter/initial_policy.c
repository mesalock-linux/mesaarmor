/*
#! SELinux Policy Editor, a simple editor for SELinux policies
#! Copyright (C) 2003 Hitachi Software Engineering Co., Ltd.
#! Copyright (C) 2005, 2006 Yuichi Nakamura
#! 
#! This program is free software; you can redistribute it and/or modify
#! it under the terms of the GNU General Public License as published by
#! the Free Software Foundation; either version 2 of the License, or
#! (at your option) any later version.
#! 
#! This program is distributed in the hope that it will be useful,
#! but WITHOUT ANY WARRANTY; without even the implied warranty of
#! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#! GNU General Public License for more details.
#! 
#! You should have received a copy of the GNU General Public License
#! along with this program; if not, write to the Free Software
#! Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*
Yuichi Nakamura <ynakam@gwu.edu>
Modification for Fedora Core4 , 
separated distribution dependent codes 
support for converter.conf
and  many fixs after 2005*/

/*Modification for Fedora Core2 Takefumi Onabuta <onabuta@selinux.gr.jp>*/
/* $Id: initial_policy.c,v 1.4 2006/04/13 18:29:24 ynakam Exp $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <selinux/context.h>
#include "initial_policy.h"
#include "security_class.h"
#include "file_label.h"
#include <seedit/common.h>
#include "global.h"

char *attribute_list[MAX_ATTR];

CONVERTER_CONF converter_conf;



/**
 *  @name:	declare_attributes
 *  @about:	declare default attribute
 *  @args:	outfp (FILE *) -> output file descripter
 *  @return:	none
 */
void
declare_attributes(FILE *outfp){	
  FILE *fp;
  char buf[BUFSIZE];  
  char *work;
  char *identifier;
  char *attr;
  char *filename = get_base_policy_files()->attribute_te;
  int i=0;
  int attr_num=0;
  /*declare*/
  include_file(filename, outfp);
  
  /*construct attribute_list*/
  for(i = 0; i<MAX_ATTR;i++)
    attribute_list[i]=NULL;
 
  if ((fp=fopen(filename,"r")) == NULL){
    fprintf(stderr, "file open error %s\n", filename);
    exit(1);
  }
  while (fgets(buf, sizeof(buf), fp) != NULL){	
    work = strdup(buf);      
    identifier = get_nth_tok(work," \t", 1);
        if(identifier == NULL){
	  free(work);
	  continue;
	}
	if(strcmp(identifier,"attribute") == 0){
	  free(work);
	  work = strdup(buf);
	  attr = get_nth_tok(work," \t", 2);
	  chop_str(attr," \t\n;");
	  attribute_list[attr_num] = strdup(attr);
	  attr_num++;
	  if(attr_num == MAX_ATTR){
	    fprintf(stderr,"Too many attributes are declared. Modify MAX_ATTR and recompile\n");
	    exit(1);
	  }
	  free(work);
	}
  }  

  
  fclose(fp); 
}

/**
 *  @name:	declare_initial_types
 *  @about:	declare default types used in types.te and converter.conf(supported_fs)
 *  @args:	outfp (FILE *) -> output file pointer
 *  @return:	none
 */
void declare_initial_types(FILE *outfp){	
	FILE *fp;
	char buf[BUFSIZE];
	char *types_te;
	char *work;
	char *identifier;
	char *type;
	int i;
	char **netif_list;
	
	/*
	  output contents in types.te
	  register types in types.te to label table
	 */
	types_te = get_base_policy_files()->types_te;

	if ((fp=fopen(types_te,"r")) == NULL) {	  
	  fprintf(stderr, "file open error %s\n", types_te);
	  exit(1); 
	}
	
	while (fgets(buf, sizeof(buf), fp) != NULL) {
	  /*output contents in types.te*/
	  fprintf(outfp, "%s", buf);
	  /*register types in types.te to label table*/
	  work = strdup(buf);
	  identifier = get_nth_tok(work," \t", 1);
	  if(identifier == NULL)    {
	    free(work);
	    continue;
	  }
	    if(strcmp(identifier,"type") == 0)  {
	      free(work);
	      work = strdup(buf);
	      type = get_nth_tok(work," \t", 2);
	      
	      if(type != NULL) {
		type = get_nth_tok(type,",",1);
	      }
	      if(type!=NULL) {
		if(register_label_table(type) == -2)
		  printf("In types.te:type %s is already registered\n", type);
	      }
	    }
	    free(work);
	}

	fprintf(outfp,"##type for supported file systems\n");
	for(i = 0; converter_conf.supported_fs_list[i] != NULL ; i++){
	  type = joint_str(converter_conf.supported_fs_list[i],"_t");	  
	  fprintf(outfp, "type %s,file_type,fs_type;\n", type);
	  
	  if(register_label_table(type) == -2){
	    printf("In converter.conf,supported_fs:type %s is already registered\n", type);
	    exit(1);
	  }
	  
	  free(type);
	  
	}
	fprintf(outfp,"##end of type for supported file systems\n");
	
	fprintf(outfp,"## type for Network Interface Card\n");
	netif_list = converter_conf.netif_name_list;
	for(i=0; netif_list[i]!=NULL;i++){
	  type = joint_str(netif_list[i],"_t");
	  if(register_label_table(type) == -2){
	    fprintf(stderr,"Error:In converter.conf,netif_name_list:type %s is already registered\n", type);
	    exit(1);
	  }
	  fprintf(outfp, "type %s,netif_type;\n", type);
	}  
	fprintf(outfp,"## endof type for Network Interface Card\n");

	fclose(fp);
	return;
}





/**
 *  @name:	declare_initial_constrains
 *  @about:	print dummy constrains
 *  @args:	outfp (FILE *) -> output file descripter
 *  @return:	none
 */
void
declare_initial_constrains(FILE *outfp)
{
	fprintf(outfp, "##dummy constrain to satisfy the syntax of policy\n");
	fprintf(outfp, "constrain process transition\n( u1 == u2 or u1 != u2 );\n\n\n");
}




/**
 *  @name:	default_allow
 *  @about:	print default allow 
 *  @args:	outfp (FILE *) -> output file descripter
 *  @return:	none
 */
void
default_allow(FILE *outfp)
{
  fprintf(outfp,"##Following are allowed in simplifed policy by default\n");
  include_file(get_base_policy_files()->seedit_macros_te, outfp);
  include_file(get_base_policy_files()->custom_macros_te, outfp);
  include_file(get_base_policy_files()->default_te, outfp);
  include_file(get_base_policy_files()->unsupported_te, outfp);
}



static BASEPOLICY *base_files = NULL;
void set_base_policy_files(char *dir, char *aosp_sepolicy_dir, char *device_sepolicy_dir){
  if(base_files == NULL )
    {
      base_files = (BASEPOLICY *)malloc(sizeof(BASEPOLICY));
      if(base_files == NULL)
	{
	  perror("malloc");
	  exit(1);
	}
    }

  /* basedir */
  base_files->converter_conf = mk_fullpath(dir, CONVERTER_CONF_FILE); 

  /* aosp sepolicy */
  base_files->security_class = mk_fullpath(aosp_sepolicy_dir,SECURITY_CLASS_FILE);
  base_files->initial_sids = mk_fullpath(aosp_sepolicy_dir,INITIAL_SIDS_FILE);
  base_files->access_vectors = mk_fullpath(aosp_sepolicy_dir, ACCESS_VECTORS_FILE);
  base_files->global_macros = mk_fullpath(aosp_sepolicy_dir, GLOBAL_MACROS_FILE);
  base_files->neverallow_macros = mk_fullpath(aosp_sepolicy_dir,NEVERALLOW_MACROS_FILE);
  base_files->mls_macros = mk_fullpath(aosp_sepolicy_dir,MLS_MACROS_FILE);
  base_files->mls = mk_fullpath(aosp_sepolicy_dir,MLS_FILE);
  base_files->policy_capabilities = mk_fullpath(aosp_sepolicy_dir,POLICY_CAPABILITIES_FILE);
  base_files->te_macros = mk_fullpath(aosp_sepolicy_dir,TE_MACROS_FILE);
  base_files->ioctl_macros = mk_fullpath(aosp_sepolicy_dir, IOCTL_MACROS_FILE);

  base_files->roles = mk_fullpath(aosp_sepolicy_dir,ROLES_FILE);
  base_files->users = mk_fullpath(aosp_sepolicy_dir, USERS_FILE);
  base_files->initial_sid_context = mk_fullpath(aosp_sepolicy_dir, INITIAL_SID_CONTEXT_FILE);
  base_files->fs_use = mk_fullpath(aosp_sepolicy_dir, FS_USE_FILE);
  base_files->genfs_context = mk_fullpath(aosp_sepolicy_dir, GENFS_CONTEXT_FILE);
  base_files->port_context = mk_fullpath(aosp_sepolicy_dir, PORT_CONTEXT_FILE);

  base_files->default_te = mk_fullpath(dir, DEFAULT_TE_FILE);  //default permission for all domains
  base_files->dontaudit_te = mk_fullpath(dir, DONTAUDIT_TE_FILE);  //dontaudit rules from aosp
  base_files->unsupported_te = mk_fullpath(dir,UNSUPPORTED_TE_FILE); //generated by genmacro.py
  base_files->types_te = mk_fullpath(dir, TYPES_FILE);  //minimum types used for converter
  base_files->seedit_macros_te = mk_fullpath(dir,SEEDIT_MACROS_TE_FILE); //generated by genmacro.py
  base_files->custom_macros_te = mk_fullpath(dir,CUSTOM_MACROS_TE_FILE); //generated by genmacro.py
  base_files->typeattribute_te = mk_fullpath(dir,TYPEATTRIBUTE_TE_FILE); //generated by genmacro.py
  base_files->svcattribute_te = mk_fullpath(dir,SVCATTRIBUTE_TE_FILE); //generated by genmacro.py
  base_files->attribute_te = mk_fullpath(dir, ATTRIBUTE_FILE);


  /* device sepolicy */
  

  return;
}

BASEPOLICY *get_base_policy_files(){
  return base_files;
}

void free_base_policy_files(){
  free(base_files);
}



void parse_allowadm_rule(char *buf){
  char **rulename_list;
  char delm[]=" \t";
  
  rulename_list = get_rulename_list();
  if(rulename_list == NULL)
    error_print(__FILE__, __LINE__, "bug\n");
  
  if(split_and_set_list(buf,delm,rulename_list, MAX_CLASS) == -1){
    fprintf(stderr,"Error: too many allow*_rule. Modify MAX_CLASS and recompile\n");
    exit(1);
  }
}

/*construct converter_conf.supported_fs_list;*/
void parse_supported_fs(char *buf){
  char delm[]=" \t";  
  if(split_and_set_list(buf,delm,converter_conf.supported_fs_list, MAX_FS) == -1){      
    fprintf(stderr,"Error: too many supported_fs. Modify MAX_FS and recompile\n");
    exit(1);
  }  
}

void parse_proc_mount_point(char *buf){
  char delm[]=" \t";  
  if(split_and_set_list(buf,delm,converter_conf.proc_mount_point_list, MAX_PROC_MOUNT) == -1){      
    fprintf(stderr,"Error: too many proc_mount_point. Modify MAX_PROC_MOUNT and recompile\n");
    exit(1);
  }  
}

void parse_authentication_domain(char *buf){
  char delm[]=" \t";  
  if(split_and_set_list(buf,delm,converter_conf.authentication_domain, MAX_CLASS) == -1){      
    fprintf(stderr,"Error: too many authentication_domain. Modify MAX_CLASS and recompile\n");
    exit(1);
  }  
}

void parse_homedir_list(char *buf){
  char delm[]=" \t";  
  int i;
  if(split_and_set_list(buf,delm,converter_conf.homedir_list, MAX_HOME) == -1){      
    fprintf(stderr,"Error: too many homedir_list. Modify MAX_HOME and recompile\n");
    exit(1);
  }  
  for(i=0; converter_conf.homedir_list[i]!=NULL;i++){
    strip_slash(converter_conf.homedir_list[i]);
  }

}


void parse_mcs_range_trans_entry(char *buf){
  char delm[]=" \t";  
  int i;
  if(split_and_set_list(buf,delm,converter_conf.mcs_range_trans_entry, MAX_ENTRY) == -1){      
    fprintf(stderr,"Error: too many mcs_range_trans_entry. Modify MAX_ENTRY and recompile\n");
    exit(1);
  }  
  for(i=0; converter_conf.mcs_range_trans_entry[i]!=NULL;i++){
    strip_slash(converter_conf.mcs_range_trans_entry[i]);
  }

}



void parse_file_type_trans_fs(char *buf){
  char delm[]=" \t";
  if(split_and_set_list(buf,delm,converter_conf.file_type_trans_fs_list,MAX_FS)==-1){
    fprintf(stderr,"Error: too many file_type_trans_fs. Modify MAX_FS and recompile\n");
    exit(1);
  }
}

void parse_netif(char *buf){
  char delm[]=" \t";
  if(split_and_set_list(buf,delm,converter_conf.netif_name_list,MAX_NETIF)==-1){
    fprintf(stderr,"Error: too many netif_name. Modify MAX_NETIF and recompile\n");
    exit(1);
  }

}

void parse_converter_conf(){
  FILE *fp;
  char buf[BUFSIZE];
  char *filename;
  char *identifier = NULL;
  char * file = NULL;
  char * label = NULL;
  int force_label_num = 0;
  FORCE_LABEL *fl;

  filename = get_base_policy_files()->converter_conf;
  if ((fp=fopen(filename,"r")) == NULL){	  
    fprintf(stderr, "file open error %s\n", filename);
    exit(1); 
  }

  while(fgets(buf, sizeof(buf), fp)){
  
    chop_nl(buf);
    identifier = get_nth_tok_alloc(buf," \t", 1);
    if(identifier == NULL)
      continue;

    if(strcmp(identifier, "force_label") == 0){
      file = get_nth_tok_alloc(buf, " \t", 2);
      label = get_nth_tok_alloc(buf, " \t", 3);
      if(file == NULL || label == NULL){
	fprintf(stderr , "Syntax error in converter.conf\n");
	exit(1);
      }
      fl = (FORCE_LABEL *)my_malloc(sizeof(FORCE_LABEL));
      
      fl->filename = file;
      fl->type = label;
      converter_conf.force_label_list[force_label_num] = fl;
      force_label_num ++;  
      if(force_label_num >= MAX_FORCE){
	fprintf(stderr, "Error!:force_label_list is short, modify MAX_FORCE and recompile\n");
	exit(1);
      }
      converter_conf.force_label_list[force_label_num] = NULL;
    }else if(strcmp(identifier, "supported_fs") == 0){      
      parse_supported_fs(buf);
    }else if(strcmp(identifier, "allowpriv_rule") == 0){
      parse_allowadm_rule(buf);    
    }else if(strcmp(identifier, "file_type_trans_fs")==0){
      parse_file_type_trans_fs(buf);
    }else if(strcmp(identifier, "netif_name")==0){
      parse_netif(buf);
    }else if(strcmp(identifier,"proc_mount_point")==0){
      parse_proc_mount_point(buf);	       
    }else if(strcmp(identifier,"authentication_domain")==0){
      parse_authentication_domain(buf);      
    }else if(strcmp(identifier,"homedir_list")==0){
      parse_homedir_list(buf);
    }else if(strcmp(identifier,"mcs_range_trans_entry")==0){
      parse_mcs_range_trans_entry(buf);	       
    }else{
      ;
    }    
    free(identifier);
  } 
	
  fclose(fp);

}



/*get pointer to list that stores string names that can be used in allowpriv*/
char **get_rulename_list(){
  char **rule_list = NULL;
  rule_list = converter_conf.allowpriv_class_list;
  return rule_list;
}
