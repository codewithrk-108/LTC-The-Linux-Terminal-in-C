#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <langinfo.h>
#include <grp.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
/*
Author : Rohan Kumar
College : IIIT Hyderabad
*/
// AHACODEJH
int bhura;
char extra_string[300] = "";
char init_string[300] = "";

char bg_name[100];
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define COLOR_BOLD "\e[1m"
#define COLOR_OFF "\e[m"

typedef struct proccess
{
	int pid;
	char pname[100];
	int pno;
} Proccess;

char history[25][200];
char his_prev[200];
int count_hist = 0;
char home_path[1000];
char target[1000];
int handler = 0;
char fg_add[200] = "";
int ext_flag = 0;
char sleep_s[100] = "";

Proccess bg_proccess[100];
Proccess fg_proccess[100];

int child_bg_process = 0;
int count_fg_process = 0;
char buffer[200];
int interuppt = 0;

void bg(char *inst)
{

	int it = 0;
	char *pass_2d[100];
	char *token = strtok(inst, " \t\n");
	while (token != NULL)
	{
		pass_2d[it] = token;
		token = strtok(NULL, " \t\n");
		it++;
	}
	pass_2d[it] = NULL;
	int id = fork();
	if (id != 0)
	{
		bg_proccess[child_bg_process].pid = id;
		strcpy(bg_proccess[child_bg_process].pname, bg_name);
		child_bg_process++;
	}
	else
	{
		setpgrp();
		int ret = execvp(pass_2d[0], pass_2d);
		if (ret == -1)
		{
			perror("Error:");
			exit(1);
		}
		else
		{
			exit(0);
		}
	}
}

void pinfo_shell()
{
	char *sup;
	char *token = strtok(NULL, " \t\n");
	sup = token;
	while (sup != NULL)
	{
		sup = strtok(NULL, " \t\n");
	}
	FILE *f_ex;
	char p[200];
	if (token == NULL)
	{
		strcpy(p, "/proc/self/status");
		f_ex = fopen("/proc/self/status", "r");
	}
	else
	{
		strcpy(p, "/proc/");
		strcat(p, token);
		strcat(p, "/status");
		f_ex = fopen(p, "r");
	}
	if (f_ex == NULL)
	{
		printf("NO PID\n");
		return;
	}
	char pinf[1000][100];
	int it = 0;
	int a = 0;
	char output[100][1000];
	char store[40];
	int f = 0;
	while (fscanf(f_ex, "%s", pinf[it]) != EOF)
	{
		if (f == 1)
		{
			strcpy(store, pinf[it]);
			f = 0;
		}
		if (strcmp(pinf[it], "Tgid:") == 0)
			f = 1;

		if (f == 2)
		{
			// printf("ok\n");
			sprintf(output[a], "pid : %s", pinf[it]);
			if (strcmp(pinf[8], pinf[it]) == 0)
			{
				strcat(output[0], "+");
			}
			a++;
			f = 0;
		}
		if (strcmp(pinf[it], "Pid:") == 0)
			f = 2;
		if (f == 3)
		{
			sprintf(output[a], "proccess Status : %s", pinf[it]);
			if (strcmp(pinf[it], store) == 0)
			{
				strcat(output[a], "+");
			}
			a++;
			f = 0;
		}

		if (strcmp(pinf[it], "State:") == 0)
			f = 3;

		if (f == 4)
		{
			sprintf(output[a], "memory : %s", pinf[it]);
			a++;
			f = 0;
		}

		if (strcmp(pinf[it], "VmSize:") == 0)
			f = 4;
		it++;
	}

	for (int i = 0; i < a; i++)
	{
		printf("%s\n", output[i]);
	}
	char fg[100];
	char path[100];
	char exec_file[100] = "/proc/";
	if (token == NULL)
		strcat(exec_file, "self");
	else
		strcat(exec_file, token);
	strcat(exec_file, "/exe\0");
	ssize_t length = readlink(exec_file, path, 100);
	path[length] = '\0';
	if (length > 0)
	{
		if (strncmp(home_path, path, strlen(home_path)) == 0)
		{
			char dir[100] = "~";
			strcat(dir, path + strlen(home_path));
			printf("executable path : %s\n", dir);
		}
		else
			printf("executable path : %s\n", path);
	}
	else
	{
		perror(path);
		return;
	}
}

void discover_recurse(char path[], int flag)
{
	char cs[200];

	getcwd(cs, sizeof(cs));
	char pt[1000];
	strcpy(pt, path);
	struct dirent *file_pt;
	DIR *directory = opendir(cs);

	if (flag == 1 || flag == -1 || flag == -3)
	{
		printf("%s\n", path);
	}
	while ((file_pt = readdir(directory)) != NULL)
	{
		char cw[200];
		strcpy(cw, cs);
		strcat(cw, "/");
		strcat(cw, file_pt->d_name);
		if (chdir(cw) != -1 && file_pt->d_name[0] != '.')
		{
			if (flag == 4)
			{
				if (strcmp(target, file_pt->d_name) == 0)
				{
					printf("%s/%s\n", path, file_pt->d_name);
					return;
				}
			}

			char orig[200];
			strcpy(orig, pt);
			strcat(pt, "/");
			strcat(pt, file_pt->d_name);
			discover_recurse(pt, flag);
			strcpy(pt, orig);
			chdir(cs);
		}
		else
		{
			if (file_pt->d_name[0] != '.')
			{
				if (flag == -2 || flag == 1 || flag == -3)
				{
					printf("%s/%s\n", path, file_pt->d_name);
				}
				// printf("%s/%s\n", path, file_pt->d_name);
				if (flag == 4)
				{
					if (strcmp(target, file_pt->d_name) == 0)
					{
						printf("%s/%s\n", path, file_pt->d_name);
						return;
					}
				}
			}
		}
		chdir(cs);
	}
}

int max(int a, int b)
{
	return (a >= b) ? a : b;
}

void history_shell(int hist_iter)
{
	for (int i = max(hist_iter - 10, 0); i < hist_iter; i++)
	{
		printf("%s\n", history[i]);
	}
}

void swap(int *p1, int *p2)
{
	int temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

int cte = 0;
int ggg = 0;
int fgbit = 0;
char orignal[1000];
char prev[1000];
char dupi[1000];
char pwrd_dup[1000];
int tot = 0;
char path[100][1000];
char cwd[2000];
char prwd[1000];

void shell_prompt(char *home_path)
{
	char ext_s[100] = "";
	char *username = getenv("USER");
	char new_path[100];
	int change_in_path = 0;
	getcwd(new_path, sizeof(new_path));
	for (int i = 0; i < strlen(new_path); i++)
	{
		if (new_path[i] != home_path[i] && i < strlen(home_path))
			change_in_path = 1;
	}
	new_path[strlen(new_path)] = '\0';
	if (ext_flag == 1)
	{
		strcpy(ext_s, sleep_s);
	}
	if (!change_in_path && strlen(home_path) <= strlen(new_path))
		printf(COLOR_BOLD YELLOW "%s@%s" RESET COLOR_BOLD ":" CYAN "~%s %s" RESET "$ ", username, username, &new_path[strlen(home_path)], ext_s);
	else
		printf(COLOR_BOLD YELLOW "%s@%s" RESET COLOR_BOLD ":" CYAN "%s-%s" RESET "$ ", username, username, new_path, ext_s);
	strcpy(sleep_s, "");
	ext_flag = 0;
}

void handler_sigchld(int sig)
{
	int find_status;
	int store = 0;
	int flag = 0;
	int pid = waitpid(-1, &find_status, WNOHANG);
	// printf("%d\n",child_bg_process);
	for (int i = 0; i < child_bg_process; i++)
	{
		if (bg_proccess[i].pid == pid)
		{
			interuppt = 1;
			flag = 1;
			store = i;
			int code = WEXITSTATUS(find_status);
			if (code == 0)
			{
				printf("\n%s with pid %d exited normally\n", bg_proccess[i].pname, bg_proccess[i].pid);
			}
			else
			{
				printf("\n%s with pid %d exited abnormally\n", bg_proccess[i].pname, bg_proccess[i].pid); // fflush(stdin);
			}
			break;
		}
	}
	if (flag)
	{
		for (int k = store; k < child_bg_process - 1; k++)
		{
			bg_proccess[k] = bg_proccess[k + 1];
		}
		child_bg_process--;
		flag = 0;
	}
	// to reorder te proccesses
}

void echo_shell()
{
	char *token;
	char ouput[10005];
	token = strtok(NULL, " \t\n");
	if (token == NULL)
	{
		printf("\n");
		return;
	}
	strcpy(ouput, token);
	while (token != NULL)
	{
		token = strtok(NULL, " \t\n");
		if (token == NULL)
			break;
		strcat(ouput, " ");
		strcat(ouput, token);
	}
	printf("%s\n", ouput);
}

void shell_prakat_ho(int flag, char *directory)
{
	char *username = getenv("USER");
	cwd[0] = '\0';
	getcwd(cwd, sizeof(cwd));
	strcpy(dupi, cwd);

	prwd[0] = '\0';

	if (cte == 0)
	{
		strcpy(orignal, cwd);
	}
	int i = strlen(cwd);
	if (flag == 5)
	{
		if (chdir(directory) != -1)
			strcpy(prwd, directory);
		else
			printf("No such file or directory exists\n");
		chdir(directory);
	}
	else if (flag == 4)
	{
		char ne[2000];
		if (directory[0] == '/')
		{
			strcpy(ne, orignal);
			strcat(ne, directory);
			if (chdir(ne) != -1)
			{
				chdir(prwd);
				strcpy(prwd, ne);
			}
			else
				printf("No such file or directory exists\n");
		}
		else
		{
			strcpy(prwd, orignal);
			chdir(prwd);
		}
	}
	else if (flag == 3)
	{
		if (cte == 1)
		{
			printf("No previous path found\n");
			shell_prakat_ho(0, "");
			return;
		}
		else
		{
			printf("%s\n", prev);
			strcpy(prwd, prev);
			chdir(prwd);
		}
	}
	else
	{
		int j = 0;
		char *token1 = strtok(cwd, "/");
		do
		{
			if (token1 == NULL)
				break;
			strcpy(path[j], token1);
			j++;
			token1 = strtok(NULL, "/");
		} while (path[j] != NULL);
		if (cte == 0)
			tot = j;
		if (flag == 0)
		{
			strcpy(prwd, dupi);
		}
		else if (flag == 1)
		{
			int k = 0;
			prwd[0] = '\0';
			while (k < j - 1)
			{
				strcat(prwd, "/");
				strcat(prwd, path[k]);
				k++;
			}
			if (strlen(prwd) != 0)
			{
				chdir(prwd);
			}
			else
			{
				prwd[0] = '/';
				prwd[1] = '\0';
				chdir("/");
			}
		}
		if (flag == 2)
		{
			char test[1000];
			int k = 0;
			prwd[0] = '\0';
			while (k < j)
			{
				strcat(prwd, "/");
				strcat(prwd, path[k]);
				k++;
			}

			if (directory[0] != '/')
				strcat(prwd, "/");

			strcat(prwd, directory);

			if (chdir(prwd) == -1)
			{
				printf("No such File or Directory\n");
				fgbit = 1;
				shell_prakat_ho(0, "");
				return;
			}
			chdir(prwd);
		}
	}

	cte++;
	if (fgbit == 0)
		strcpy(prev, dupi);
	fgbit = 0;
}

void ls_shell()
{
	// printf("enter\n");
	char cc[100];
	getcwd(cc, sizeof(cc));
	int shell_flag = 0;
	char for_ls[1000][100];
	char lower_for_ls[1000][100];
	struct dirent *file_pt;
	DIR *directory = opendir(cc);
	if (directory == NULL)
	{
		printf("Not able to open the directory\n");
	}
	int it = 0;
	int chk_dir[1005] = {0};
	while ((file_pt = readdir(directory)) != NULL)
	{
		strcpy(for_ls[it], file_pt->d_name);
		// printf("%d\n",file_pt->d_type);
		if (file_pt->d_type == 4)
			chk_dir[it] = 1;
		for (int i = 0; i < strlen(for_ls[it]); i++)
		{
			// printf("%c\n",for_ls[it][i]);
			lower_for_ls[it][i] = for_ls[it][i];
			if (for_ls[it][i] < 97)
			{
				// printf("%c\n",for_ls[it][i]+32);
				lower_for_ls[it][i] = (for_ls[it][i] + 32);
				// printf("%c\n",(for_ls[it][i]));
			}
		}
		lower_for_ls[it][strlen(for_ls[it])] = '\0';
		// printf("%s\n",lower_for_ls[it]);
		it++;
		// printf("%s  ", file_pt->d_name);
	}

	for (int i = 0; i < it; i++)
	{
		for (int j = 1; j < it; j++)
		{
			if (strcmp(lower_for_ls[j], lower_for_ls[j - 1]) < 0)
			{
				// printf("ok\n");
				char swapper[1000];
				strcpy(swapper, for_ls[j]);
				strcpy(for_ls[j], for_ls[j - 1]);
				strcpy(for_ls[j - 1], swapper);
				swap(&chk_dir[j], &chk_dir[j - 1]);

				strcpy(swapper, lower_for_ls[j]);
				strcpy(lower_for_ls[j], lower_for_ls[j - 1]);
				strcpy(lower_for_ls[j - 1], swapper);
			}
		}
	}
	struct stat executable;
	char *token2 = strtok(NULL, " \t\n");
	char *apnatoken = strtok(NULL, " \t\n");
	if ((token2 == NULL || strcmp(token2, ".") == 0) && apnatoken == NULL)
	{
		for (int i = 0; i < it; i++)
		{
			if (for_ls[i][0] != '.')
			{
				stat(for_ls[i], &executable);
				if (chk_dir[i] == 1)
					printf(COLOR_BOLD BLUE "%s   " RESET, for_ls[i]);
				else if (executable.st_mode & S_IXUSR)
				{
					printf(COLOR_BOLD GREEN "%s   " RESET, for_ls[i]);
				}
				else
					printf("%s   ", for_ls[i]);
			}
		}
		printf("\n");
	}
	else if ((strcmp(token2, "..") == 0 || strcmp(token2, "~") == 0) && apnatoken == NULL)
	{
		// printf("ok\n");
		char for_ls1[1000][100];
		char lower_for_ls1[1000][100];
		char yy[100];
		char n[100];
		getcwd(yy, sizeof(yy));
		// strcpy();
		shell_flag = 1;
		shell_prakat_ho(shell_flag, "");
		// printf("%s\n")
		struct dirent *file_pt;
		DIR *directory;
		getcwd(n, sizeof(n));
		if (strcmp(token2, "..") == 0)
			directory = opendir(n);
		else
		{
			directory = opendir(home_path);
		}
		if (directory == NULL)
		{
			printf("Not able to open the directory\n");
		}
		int it = 0;
		int chk_dir[1005] = {0};
		while ((file_pt = readdir(directory)) != NULL)
		{
			strcpy(for_ls1[it], file_pt->d_name);
			if (file_pt->d_type == 4)
				chk_dir[it] = 1;
			for (int i = 0; i < strlen(for_ls1[it]); i++)
			{
				// printf("%c\n",for_ls[it][i]);
				lower_for_ls1[it][i] = for_ls1[it][i];
				if (for_ls1[it][i] < 97)
				{
					// printf("%c\n",for_ls[it][i]+32);
					lower_for_ls1[it][i] = (for_ls1[it][i] + 32);
					// printf("%c\n",(for_ls[it][i]));
				}
			}
			lower_for_ls1[it][strlen(for_ls1[it])] = '\0';
			// printf("%s\n",lower_for_ls[it]);
			it++;
		}
		for (int i = 0; i < it; i++)
		{
			for (int j = 1; j < it; j++)
			{
				if (strcmp(lower_for_ls1[j], lower_for_ls1[j - 1]) < 0)
				{
					// printf("ok\n");
					char swapper[1000];
					strcpy(swapper, for_ls1[j]);
					strcpy(for_ls1[j], for_ls1[j - 1]);
					strcpy(for_ls1[j - 1], swapper);
					swap(&chk_dir[j], &chk_dir[j - 1]);

					strcpy(swapper, lower_for_ls1[j]);
					strcpy(lower_for_ls1[j], lower_for_ls1[j - 1]);
					strcpy(lower_for_ls1[j - 1], swapper);
				}
			}
		}

		for (int i = 0; i < it; i++)
		{
			if (for_ls1[i][0] != '.')
			{
				stat(for_ls[i], &executable);
				if (chk_dir[i] == 1)
					printf(COLOR_BOLD BLUE "%s   \n" RESET, for_ls1[i]);
				else if (executable.st_mode & S_IXUSR)
				{
					printf(COLOR_BOLD GREEN "%s   \n" RESET, for_ls1[i]);
				}
				else
					printf("%s   \n", for_ls1[i]);
			}
		}
		// printf("%s\n", yy);
		chdir(yy);
		shell_prakat_ho(0, "");
	}
	else
	{
		// printf("ok\n");
		int flag_iter = 0;
		char flags[100][100];
		char directry[100][100];
		int direct_i = 0;
		strcpy(flags[flag_iter++], token2);
		char temp[100];
		while (apnatoken != NULL)
		{
			strcpy(flags[flag_iter++], apnatoken);
			apnatoken = strtok(NULL, " \t\n");
		}
		int fg = -2;
		char low_directry[100][1000];
		// printf("%d\n",flag_iter);
		for (int i = 0; i < flag_iter; i++)
		{
			// printf("%s\n",flags[i]);
			if (strcmp(flags[i], "-a") == 0)
			{
				if (fg < -1)
					fg = -1;
				if (fg == 1)
				{
					fg = 2;
				}
			}
			else if (strcmp(flags[i], "-l") == 0)
			{
				if (fg < 1 && fg != -1)
					fg = 1;
				if (fg == -1)
				{
					fg = 2;
				}
			}
			else if (strcmp(flags[i], "-al") == 0 || strcmp(flags[i], "-la") == 0)
			{
				if (fg < 2)
					fg = 2;
			}
			else
			{
				if (flags[i][0] == '-')
					fg = -3;
				else
				{
					strcpy(directry[direct_i], flags[i]);
					for (int i = 0; i < strlen(directry[direct_i]); i++)
					{
						// printf("%c\n",directry[][i]);
						low_directry[direct_i][i] = directry[direct_i][i];
						if (directry[direct_i][i] < 97)
						{
							// printf("%c\n",for_ls[it][i]+32);
							low_directry[direct_i][i] = (directry[direct_i][i] + 32);
							// printf("%c\n",(for_ls[it][i]));
						}
					}
					low_directry[direct_i][strlen(directry[direct_i])] = '\0';
					direct_i++;
				}
			}
		}
		if (direct_i > 0)
		{
			// printf("%d\n",direct_i);
			for (int i = 0; i < direct_i; i++)
			{
				for (int j = 1; j < direct_i; j++)
				{
					// printf("%s %s\n",low_directry[j],low_directry[j-1]);
					if (strcmp(low_directry[j], low_directry[j - 1]) < 0)
					{
						// printf("ok\n");
						char swapper[1000];
						strcpy(swapper, directry[j]);
						strcpy(directry[j], directry[j - 1]);
						strcpy(directry[j - 1], swapper);
						swap(&chk_dir[j], &chk_dir[j - 1]);

						strcpy(swapper, low_directry[j]);
						strcpy(low_directry[j], low_directry[j - 1]);
						strcpy(low_directry[j - 1], swapper);
					}
				}
			}
			// printf("ok\n");

			int er = 0;
			char dudh[200];
			while (er < direct_i)
			{
				char yy[200];
				getcwd(yy, sizeof(yy));
				if (directry[er][0] == '~')
				{
					strcpy(dudh, home_path);
					strcat(dudh, &directry[er][1]);
				}
				else
				{
					strcpy(dudh, directry[er]);
				}
				// printf("%s\n",dudh);
				if (chdir(dudh) == -1)
				{

					if (access(dudh, F_OK) == 0)
					{
						// file exists

						printf("%s:\n", directry[er]);
					}
					else
					{
						printf("%s : No such file or Directory found\n", directry[er]);
						er++;
						continue;
					}

					// printf("No such file or Directory found\n");
					if (fg == -3)
					{
						printf("Error : command not found\n");
					}
					if (fg == -2 || fg == -1)
					{

						printf("%s   ", directry[er]);
						printf("\n");
					}

					if (fg == 1 || fg == 2)
					{
						struct stat dir;
						for (int i = 0; i < 1; i++)
						{
							int for_per = 0;
							if (dudh[0] != '.' || fg == 2)
							{
								stat(dudh, &dir);
								char permissions[50];
								if (S_ISDIR(dir.st_mode))
								{
									permissions[for_per++] = 'd';
								}
								else
								{
									permissions[for_per++] = '-';
								}
								if (dir.st_mode & S_IRUSR)
									permissions[for_per++] = 'r';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IWUSR)
									permissions[for_per++] = 'w';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IXUSR)
									permissions[for_per++] = 'x';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IRGRP)
									permissions[for_per++] = 'r';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IWGRP)
									permissions[for_per++] = 'w';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IXGRP)
									permissions[for_per++] = 'x';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IROTH)
									permissions[for_per++] = 'r';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IWOTH)
									permissions[for_per++] = 'w';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IXOTH)
									permissions[for_per++] = 'x';
								else
									permissions[for_per++] = '-';
								permissions[for_per++] = '\0';
								int nlink = dir.st_nlink;
								int size = dir.st_size;
								// struct stat sb;
								struct tm *t;
								char date[500];
								t = localtime(&dir.st_mtime);
								strftime(date, 500, nl_langinfo(D_T_FMT), t);
								for (int i = 4; i < 16; i++)
								{
									date[i - 4] = date[i];
								}
								date[12] = '\0';
								stat(dudh, &executable);
								if (chk_dir[i] == 1)
									printf("%10s %5d %10s %10s %10d %15s " COLOR_BOLD BLUE "%-10s\n" RESET, permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, directry[er]);
								else if (executable.st_mode & S_IXUSR)
								{
									printf("%10s %5d %10s %10s %10d %15s " COLOR_BOLD GREEN "%-10s\n" RESET, permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, directry[er]);
								}
								else
									printf("%10s %5d %10s %10s %10d %15s %-10s\n", permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, directry[er]);
							}
						}
					}
				}

				else
				{
					char for_ls1[1000][100];
					char lower_for_ls1[1000][100];
					shell_flag = 1;
					// shell_prakat_ho(shell_flag, "");
					struct dirent *file_pt;
					DIR *directory;
					if (direct_i != 1)
						printf("%s:\n", directry[er]);
					struct stat dr;
					char pth[100];
					// printf("total:%d\n",)
					int totle = 0;
					if (directry[er][0] == '/')
					{
						directory = opendir(directry[er]);
						stat(directry[er], &dr);
						if (fg > -1)
							printf("total:%ld\n", dr.st_blocks);
					}
					else
					{
						getcwd(pth, sizeof(pth));
						directory = opendir(pth);
						stat(pth, &dr);
						// printf("total:%ld\n", dr.st_blocks);
					}
					int it = 0;
					int chk_dir[1005] = {0};
					// printf("ok\n");
					struct stat hio;
					// printf("%s\n",readdir(directory)->d_name);
					while ((file_pt = readdir(directory)) != NULL)
					{
						char gh[200];
						strcpy(gh, pth);
						strcat(gh, "/");
						strcat(gh, file_pt->d_name);
						// printf("ok\n");
						stat(gh, &hio);
						if (file_pt->d_name[0] != '.')
						{
							totle += hio.st_blocks;
							// 	// printf("%ld\n", hio.st_blocks);
						}
						strcpy(for_ls1[it], file_pt->d_name);
						if (file_pt->d_type == 4)
							chk_dir[it] = 1;
						for (int i = 0; i < strlen(for_ls1[it]); i++)
						{
							// printf("%c\n",for_ls[it][i]);
							lower_for_ls1[it][i] = for_ls1[it][i];
							if (for_ls1[it][i] < 97)
							{
								// printf("%c\n",for_ls[it][i]+32);
								lower_for_ls1[it][i] = (for_ls1[it][i] + 32);
								// printf("%c\n",(for_ls[it][i]));
							}
						}
						lower_for_ls1[it][strlen(for_ls1[it])] = '\0';
						// printf("%s\n",lower_for_ls[it]);
						it++;

						// printf("\n");
						// printf("%s  ", file_pt->d_name);
					}
					if (fg > -1)
						printf("total:%d\n", totle / 2);

					// printf("ok\n");
					for (int i = 0; i < it; i++)
					{
						for (int j = 1; j < it; j++)
						{
							if (strcmp(lower_for_ls1[j], lower_for_ls1[j - 1]) < 0)
							{
								// printf("ok\n");
								char swapper[1000];
								strcpy(swapper, for_ls1[j]);
								strcpy(for_ls1[j], for_ls1[j - 1]);
								strcpy(for_ls1[j - 1], swapper);
								swap(&chk_dir[j], &chk_dir[j - 1]);

								strcpy(swapper, lower_for_ls1[j]);
								strcpy(lower_for_ls1[j], lower_for_ls1[j - 1]);
								strcpy(lower_for_ls1[j - 1], swapper);
							}
						}
					}

					// printf("%d\n",fg);
					// shell_prakat_ho(0, "");
					if (fg == -3)
					{
						printf("Error : command not found\n");
					}
					// printf("ok\n");
					if (fg == -2)
					{
						for (int i = 0; i < it; i++)
						{
							if (for_ls1[i][0] != '.')
							{
								stat(for_ls1[i], &executable);
								if (chk_dir[i] == 1)
									printf(COLOR_BOLD BLUE "%s   " RESET, for_ls1[i]);
								else if (executable.st_mode & S_IXUSR)
								{
									printf(COLOR_BOLD GREEN "%s   " RESET, for_ls1[i]);
								}
								else
									printf("%s   ", for_ls1[i]);
							}
						}
						printf("\n");
					}
					if (fg == -1)
					{
						for (int i = 0; i < it; i++)
						{
							stat(for_ls1[i], &executable);
							if (chk_dir[i] == 1)
								printf(COLOR_BOLD BLUE "%s   " RESET, for_ls1[i]);
							else if (executable.st_mode & S_IXUSR)
							{
								printf(COLOR_BOLD GREEN "%s   " RESET, for_ls1[i]);
							}
							else
								printf("%s   ", for_ls1[i]);
						}
						printf("\n");
					}
					if (fg == 1 || fg == 2)
					{
						struct stat dir;
						for (int i = 0; i < it; i++)
						{
							int for_per = 0;
							if (for_ls1[i][0] != '.' || fg == 2)
							{
								stat(for_ls1[i], &dir);
								char permissions[50];
								if (S_ISDIR(dir.st_mode))
								{
									permissions[for_per++] = 'd';
								}
								else
								{
									permissions[for_per++] = '-';
								}
								if (dir.st_mode & S_IRUSR)
									permissions[for_per++] = 'r';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IWUSR)
									permissions[for_per++] = 'w';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IXUSR)
									permissions[for_per++] = 'x';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IRGRP)
									permissions[for_per++] = 'r';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IWGRP)
									permissions[for_per++] = 'w';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IXGRP)
									permissions[for_per++] = 'x';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IROTH)
									permissions[for_per++] = 'r';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IWOTH)
									permissions[for_per++] = 'w';
								else
									permissions[for_per++] = '-';

								if (dir.st_mode & S_IXOTH)
									permissions[for_per++] = 'x';
								else
									permissions[for_per++] = '-';
								permissions[for_per++] = '\0';
								int nlink = dir.st_nlink;
								int size = dir.st_size;
								// struct stat sb;
								struct tm *t;
								char date[500];
								t = localtime(&dir.st_mtime);
								strftime(date, 500, nl_langinfo(D_T_FMT), t);
								for (int i = 4; i < 16; i++)
								{
									date[i - 4] = date[i];
								}
								date[12] = '\0';
								stat(for_ls1[i], &executable);
								if (chk_dir[i] == 1)
									printf("%10s %5d %10s %10s %10d %15s " COLOR_BOLD BLUE "%-10s\n" RESET, permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, for_ls1[i]);
								else if (executable.st_mode & S_IXUSR)
								{
									printf("%10s %5d %10s %10s %10d %15s " COLOR_BOLD GREEN "%-10s\n" RESET, permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, for_ls1[i]);
								}
								else
									printf("%10s %5d %10s %10s %10d %15s %-10s\n", permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, for_ls1[i]);
							}
						}
					}
				}
				chdir(yy);
				er++;
			}
		}
		else
		{
			if (fg == -3)
			{
				printf("Error : command not found\n");
			}
			if (fg == -1)
			{
				for (int i = 0; i < it; i++)
				{
					stat(for_ls[i], &executable);
					if (chk_dir[i] == 1)
						printf(COLOR_BOLD BLUE "%s   " RESET, for_ls[i]);
					else if (executable.st_mode & S_IXUSR)
					{
						printf(COLOR_BOLD GREEN "%s   " RESET, for_ls[i]);
					}
					else
						printf("%s   ", for_ls[i]);
				}
				printf("\n");
			}
			if (fg == 1 || fg == 2)
			{
				struct stat dir;
				for (int i = 0; i < it; i++)
				{
					int for_per = 0;
					if (for_ls[i][0] != '.' || fg == 2)
					{
						stat(for_ls[i], &dir);
						char permissions[50];
						if (S_ISDIR(dir.st_mode))
						{
							permissions[for_per++] = 'd';
						}
						else
						{
							permissions[for_per++] = '-';
						}
						if (dir.st_mode & S_IRUSR)
							permissions[for_per++] = 'r';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IWUSR)
							permissions[for_per++] = 'w';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IXUSR)
							permissions[for_per++] = 'x';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IRGRP)
							permissions[for_per++] = 'r';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IWGRP)
							permissions[for_per++] = 'w';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IXGRP)
							permissions[for_per++] = 'x';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IROTH)
							permissions[for_per++] = 'r';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IWOTH)
							permissions[for_per++] = 'w';
						else
							permissions[for_per++] = '-';

						if (dir.st_mode & S_IXOTH)
							permissions[for_per++] = 'x';
						else
							permissions[for_per++] = '-';
						permissions[for_per++] = '\0';
						int nlink = dir.st_nlink;
						int size = dir.st_size;
						// struct stat sb;
						struct tm *t;
						char date[500];
						t = localtime(&dir.st_mtime);
						strftime(date, 500, nl_langinfo(D_T_FMT), t);
						for (int i = 4; i < 16; i++)
						{
							date[i - 4] = date[i];
						}
						date[12] = '\0';
						stat(for_ls[i], &executable);
						if (chk_dir[i] == 1)
							printf("%10s %5d %10s %10s %10d %15s " COLOR_BOLD BLUE "%-10s\n" RESET, permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, for_ls[i]);
						else if (executable.st_mode & S_IXUSR)
						{
							printf("%10s %5d %10s %10s %10d %15s " COLOR_BOLD GREEN "%-10s\n" RESET, permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, for_ls[i]);
						}
						else
							printf("%10s %5d %10s %10s %10d %15s %-10s\n", permissions, nlink, getpwuid(dir.st_uid)->pw_name, getgrgid(dir.st_gid)->gr_name, size, date, for_ls[i]);
					}
				}
			}
		}
	}
	// printf("exit\n");
}

void discover_shell()
{
	char fg_disc[1000][100];
	char dir_disc[1000][100];
	int disc_flag = -3;
	char *ap_tok;
	int disc_it = 0;
	int dir_it = 0;

	ap_tok = strtok(NULL, " \t\n");
	while (ap_tok != NULL)
	{
		if (ap_tok[0] == '-')
		{
			strcpy(fg_disc[disc_it], ap_tok);
			disc_it++;
		}
		else
		{
			strcpy(dir_disc[dir_it], ap_tok);
			dir_it++;
		}
		ap_tok = strtok(NULL, " \t\n");
	}
	char cc[200];
	getcwd(cc, sizeof(cc));
	for (int i = 0; i < disc_it; i++)
	{
		if (strcmp(fg_disc[i], "-f") == 0)
		{
			if (disc_flag == -1)
				disc_flag = 1;
			else
				disc_flag = -2;
		}
		if (strcmp(fg_disc[i], "-d") == 0)
		{
			if (disc_flag == -2)
				disc_flag = 1;
			else
				disc_flag = -1;
		}
	}
	int ctr = 0;
	int fl = 0;
	char check_path[200] = "";
	// check_path[0] = '\0';
	char file_n[200];
	// char d_name[200];
	int fd = 0;
	char naam[100];
	int se = 0;
	for (int i = 0; i < dir_it; i++)
	{
		if (dir_disc[i][0] == '"')
		{
			if (se == 0)
			{
				disc_flag = 4;
				// strcpy(check_path, .);
				strcpy(check_path, "./");
				dir_disc[i][strlen(dir_disc[i]) - 1] = '\0';
				strcat(check_path, &dir_disc[i][1]);
			}
			else
			{
				disc_flag = 4;
				dir_disc[i][strlen(dir_disc[i]) - 1] = '\0';
				strcpy(file_n, dir_disc[i]);
			}
		}
		else
		{
			se = 1;
			if (dir_disc[i][0] != '~')
				strcat(check_path, dir_disc[i]);
			else
			{
				fd = 1;
				strcpy(naam, dir_disc[i]);
				strcat(check_path, home_path);
				strcat(check_path, &dir_disc[i][1]);
			}
		}
	}
	// printf("%s\n",check_path);
	if (disc_flag == 4)
	{
		// printf("ok\n");
		if (chdir(check_path) != -1)
		{
			strcpy(target, &file_n[1]);
			if (fd == 1)
			{

				naam[0] = '~';
				discover_recurse(naam, 4);
				fd = 0;
			}
			else
				discover_recurse(check_path, 4);
			disc_flag = 1;
		}
		else if (access(check_path, F_OK) == 0)
		{
			printf("%s\n", check_path);
		}
		else
		{
			printf("Not Found\n");
		}
		chdir(cc);
	}
	else
	{
		chdir(cc);
		if (strcmp(check_path, "") != 0)
		{
			if (disc_flag == -3)
			{
				disc_flag = 1;
			}
			if (chdir(check_path) != -1)
			{
				if (fd == 1)
				{
					naam[0] = '~';
					discover_recurse(naam, disc_flag);
					// discover_recurse("~", disc_flag);
					fd = 0;
				}
				else
					discover_recurse(check_path, disc_flag);
			}
			else
			{
				printf("Not Found\n");
			}
		}
		else
			discover_recurse(".", disc_flag);
		chdir(cc);
	}
}

void cd_shell()
{
	int shell_flag = -1;
	char *token2 = strtok(NULL, " \t\n");
	if (token2 == NULL)
	{
		return;
	}
	int sp = 0, ep = strlen(token2) - 1;
	while (token2[sp] == ' ' || token2[sp] == '\t')
		sp++;
	while (token2[ep] == ' ' || token2[ep] == '\t')
		ep--;
	token2[ep + 1] = '\0';
	sp = 0;
	if (token2[sp] == '/')
	{
		shell_flag = 5;
		shell_prakat_ho(shell_flag, &token2[sp]);
	}
	else if (strcmp(&token2[sp], "..") == 0)
	{
		shell_flag = 1;
		shell_prakat_ho(shell_flag, "");
		// printf("exit\n");
	}
	else if (strcmp(&token2[sp], ".") == 0)
	{
		shell_flag = 0;
		shell_prakat_ho(shell_flag, "");
	}
	else if (strcmp(&token2[sp], "-") == 0)
	{
		shell_flag = 3;
		shell_prakat_ho(shell_flag, "");
	}
	else if (token2[sp] == '~')
	{
		shell_flag = 4;
		shell_prakat_ho(shell_flag, &token2[sp + 1]);
	}
	else
	{
		shell_flag = 2;
		shell_prakat_ho(shell_flag, token2);
	}
}

void print_dir()
{
	char cwd[2000];
	getcwd(cwd, sizeof(cwd));
	printf("%s\n", cwd);
}

void jobs(int flag)
{
	Proccess sorted_proccess[1000];
	for(int i=0;i < child_bg_process ; i++)
	{
		sorted_proccess[i] = bg_proccess[i];
		sorted_proccess[i].pno = i+1;
	}
	//bubble sort
	for(int i=0;i<child_bg_process ; i++)
	{
		for(int j=i+1 ; j<child_bg_process ; j++)
		{
			if(strcmp(sorted_proccess[j].pname,sorted_proccess[j-1].pname)<0)
			{
				Proccess temp = sorted_proccess[j-1];
				sorted_proccess[j-1]=sorted_proccess[j];
				sorted_proccess[j] = temp;
			}
		}
	}
	for (int i = 0; i < child_bg_process; i++)
	{
		char itos[100];
		// printf("%s --> %d\n", bg_proccess[i].pname, bg_proccess[i].pid);
		sprintf(itos, "%d", sorted_proccess[i].pid);
		FILE *f_ex;
		char p[200];
		strcpy(p, "/proc/");
		strcat(p, itos);
		strcat(p, "/status");
		f_ex = fopen(p, "r");

		if (f_ex == NULL)
		{
			printf("NO PID\n");
			return;
		}
		char pinf[1000][100];
		int it = 0;
		int a = 0;
		char output[100][1000];
		char store[40];
		int f = 0;
		int signa = 0;
		while (fscanf(f_ex, "%s", pinf[it]) != EOF)
		{
			if (f == 1)
			{
				strcpy(store, pinf[it]);
				f = 0;
			}

			if (f == 2)
			{
				sprintf(output[a], "[%s]", pinf[it]);
				a++;
				f = 0;
			}
			if (strcmp(pinf[it], "Pid:") == 0)
				f = 2;
			if (f == 3)
			{
				if (strcmp(pinf[it], "T") == 0)
					signa = 0;
				else
					signa = 1;
				f = 0;
			}

			if (strcmp(pinf[it], "State:") == 0)
				f = 3;
			it++;
		}

		if (signa == 0 && (flag == 0 || flag == 1))
			printf("[%d] Stopped %s %s\n", sorted_proccess[i].pno, sorted_proccess[i].pname, output[0]);
		if (signa == 1 && (flag == 0 || flag == 2))
			printf("[%d] Running %s %s\n", sorted_proccess[i].pno, sorted_proccess[i].pname, output[0]);
	}
}

void sig(int num1, int num2)
{
	int killnum = kill(bg_proccess[num1 - 1].pid, num2);
	if (killnum == -1)
	{
		printf("bash : no such proccess found\n");
	}
}

void send_to_fg(int num)
{
	// printf("ok\n");
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(STDIN_FILENO, bg_proccess[num - 1].pid);
	kill(bg_proccess[num - 1].pid, SIGCONT);
	for (int k = num - 1; k < child_bg_process - 1; k++)
	{
		bg_proccess[k] = bg_proccess[k + 1];
	}
	child_bg_process--;
	waitpid(-1, NULL, WUNTRACED);
	tcsetpgrp(STDIN_FILENO, getpgrp());
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
}

void resume_in_bg(int number)
{
	// printf("%d\n",child_bg_process);
	if (number - 1 < child_bg_process && number >= 1)
	{
		kill(bg_proccess[number - 1].pid, SIGTTIN);
		kill(bg_proccess[number - 1].pid, SIGCONT);
	}
	else
	{
		printf("Job Not Found\n");
	}
}

void fg(char *token_for_inst, int hist_iter)
{
	bhura = hist_iter;
	char *sup[100];
	char command_name[1000];
	int it = 0;
	if (strcmp(token_for_inst, "echo") == 0)
	{
		echo_shell();
	}
	else if (strcmp(token_for_inst, "cd") == 0)
	{
		cd_shell();
	}
	else if (strcmp(token_for_inst, "pwd") == 0)
	{
		print_dir();
	}
	else if (strcmp(token_for_inst, "ls") == 0)
	{
		ls_shell();
	}
	else if (strcmp(token_for_inst, "history") == 0)
	{
		history_shell(hist_iter);
	}
	else if (strcmp(token_for_inst, "discover") == 0)
	{
		discover_shell();
	}
	else if (strcmp(token_for_inst, "pinfo") == 0)
	{
		pinfo_shell();
	}
	else if (strcmp(token_for_inst, "jobs") == 0)
	{
		int fl = 0;
		char *g = strtok(NULL, " \t\n");
		while (g != NULL)
		{
			if (strcmp(g, "-s") == 0)
			{
				fl = 1;
			}
			if (strcmp(g, "-r") == 0)
			{
				fl = 2;
			}
			g = strtok(NULL, " \t\n");
		}
		jobs(fl);
	}
	else if (strcmp(token_for_inst, "sig") == 0)
	{
		int num1 = 0, num2 = 0;
		char *g = strtok(NULL, " \t\n");
		num1 = atoi(g);
		g = strtok(NULL, " \t\n");
		num2 = atoi(g);
		sig(num1, num2);
	}
	else if (strcmp(token_for_inst, "fg") == 0)
	{
		int num1 = 0;
		char *g = strtok(NULL, " \t\n");
		num1 = atoi(g);
		// g = strtok(NULL, " \t\n");
		// num2 = atoi(g);
		send_to_fg(num1);
	}
	else if (strcmp(token_for_inst, "bg") == 0)
	{
		int num1 = 0;
		char *g = strtok(NULL, " \t\n");
		num1 = atoi(g);
		// g = strtok(NULL, " \t\n");
		// num2 = atoi(g);
		resume_in_bg(num1);
	}
	else
	{
		sup[it++] = token_for_inst;
		char *token = strtok(NULL, " \t\n");
		if (token == NULL)
		{
			printf("Invalid Command\n");
		}
		strcpy(command_name, token_for_inst);
		while (token != NULL)
		{
			sup[it++] = token;
			token = strtok(NULL, " \t\n");
		}
		sup[it] = NULL;

		int id = fork();
		int t = time(NULL);
		if (id == 0)
		{
			execvp(command_name, sup);
		}
		int states;

		if (id != 0)
		{
			fg_proccess[count_fg_process - 1].pid = id;
			waitpid(id, &states, WUNTRACED | WCONTINUED);
			// fg_proccess[count_fg_process - 1].pid = id;
		}
		t = time(NULL) - t;
		if (t >= 1)
		{
			sprintf(sleep_s, "took %ds ", t);
			ext_flag = 1;
		}
		printf("\n");
	}
}

void pipe_fg(char **inst, int total, int hist_iter)
{
	char command_name[100];
	char *sup[1000];
	int fd[2];
	int lite = 0;
	for (int i = 0; i < total + 1; i++)
	{
		if (pipe(fd) < 0)
		{
			printf("bash : pipe not opening\n");
			return;
		}
		char *garbage = strtok(NULL, " \t\n");
		int it = 0;
		while (garbage != NULL)
		{
			garbage = strtok(NULL, " \t\n");
		}
		char *token = strtok(inst[i], " \t\n");
		if (token == NULL)
		{
			printf("Invalid Command\n");
		}
		strcpy(command_name, token);
		while (token != NULL)
		{
			// printf("%s\n", token);
			sup[it++] = token;
			token = strtok(NULL, " \t\n");
		}

		sup[it] = NULL;
		int clonei = dup(0);
		int clone = dup(1);
		int chd = fork();
		if (chd == 0)
		{
			if (i == 0)
			{
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}
			else
			{
				fd[0] = dup(lite);
				if (i >= total)
				{
					dup2(clone, 1);
					close(clone);
				}
				else
				{
					close(fd[0]);
					dup2(fd[1], STDOUT_FILENO);
					close(fd[1]);
				}
				dup2(lite, STDIN_FILENO);
				close(fd[0]);
			}
			close(fd[1]);
			execvp(command_name, sup);
			exit(1);
		}
		else
		{
			wait(NULL);
			lite = dup(fd[0]);
			close(fd[0]);
			close(fd[1]);
			dup2(clone, 1);
			dup2(clonei, 0);
		}
	}
}

void ctrlc_handler()
{
	if (count_fg_process == 0)
	{
		printf("\n");
		shell_prompt(home_path);
		fflush(stdout);
	}
	else
	{
		if (fg_proccess[count_fg_process].pid != 0)
			kill(fg_proccess[count_fg_process].pid, SIGINT);
		count_fg_process--;
	}
}

void ctrlz_handler()
{
	if (count_fg_process > 0)
	{
		if (fg_proccess[count_fg_process - 1].pid != 0)
		{
			// printf("ok");
			kill(fg_proccess[count_fg_process - 1].pid, SIGTSTP);
			bg_proccess[child_bg_process].pid = fg_proccess[count_fg_process - 1].pid;
			strcpy(bg_proccess[child_bg_process].pname, fg_proccess[count_fg_process - 1].pname);
			child_bg_process++;
			count_fg_process--;
		}
		else
		{
			count_fg_process--;
			printf("\n");
			shell_prompt(home_path);
			fflush(stdout);
		}
	}
	else
	{
		printf("\n");
		shell_prompt(home_path);
		fflush(stdout);
	}
}


void die(const char *s)
{
	perror(s);
	exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}
char *autofill = "";
char add_str[200];

// Autocomplete

void recomendations()
{
	// printf("ok\n");

	char curr[300];
	getcwd(curr, sizeof(curr));
	struct dirent *file_pt;
	DIR *directory = opendir(curr);
	struct dirent *fileptr;
	char *storage_recommendations[1000];
	int iterator = 0;
	int minima = __INT_MAX__;
	// printf("hello\n");
	// printf("jaiho%s\n",extra_string);
	while ((fileptr = readdir(directory)) != NULL)
	{
		if (strcmp(extra_string, "") == 0 || strncmp(extra_string, fileptr->d_name, strlen(extra_string)) == 0)
		{
			if (minima > strlen(fileptr->d_name))
			{
				minima = strlen(fileptr->d_name);
			}
			storage_recommendations[iterator++] = fileptr->d_name;
		}
	}
	// printf("oktill\n");
	// printf("%d\n", iterator);
	int chal_hatt = 0;
	int kr = 0, ir = 0;
	for (int j = 0; j < minima; j++)
	{
		kr = 0;
		for (int i = 0; i < iterator; i++)
		{
			if (storage_recommendations[i][j] == storage_recommendations[0][j])
			{
				kr++;
			}
		}
		if (iterator > 0)
		{
			if (kr == iterator)
			{
				// printf("ok\n");
				add_str[ir++] = storage_recommendations[0][j];
			}
			else
			{
				chal_hatt = 1;
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (chal_hatt || iterator > 1)
	{
		printf("\n");
		for (int i = 0; i < iterator; i++)
		{
			printf("%s\n", storage_recommendations[i]);
		}
	}

	add_str[ir] = '\0';
	struct stat dir;

	if (iterator == 1)
	{
		strcpy(add_str, storage_recommendations[0]);
		if(stat(storage_recommendations[0],&dir)==0)
		{
			if(S_ISDIR(dir.st_mode))
			{
				strcat(add_str,"/");
			}
			else
			{
				strcat(add_str," ");
			}
		}
	}
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");
	atexit(disableRawMode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}
char *good;
char boy[300] = "";

char *takeInput()
{
	char *inp = malloc(sizeof(char) * 100);
	char c;
	setbuf(stdout, NULL);
	enableRawMode();
	memset(inp, '\0', 100);
	int pt = 0;
	// printf("%s %s", init_string, add_str);
	// fprintf(stdin, "%s%s", init_string, add_str);
	while (read(STDIN_FILENO, &c, 1) == 1)
	{
		if (iscntrl(c))
		{
			if (c == 10)
				break;
			else if (c == 27)
			{
				char buf[3];
				buf[2] = 0;
				if (read(STDIN_FILENO, buf, 2) == 2)
				{ // length of escape code
					printf("\rarrow key: %s", buf);
				}
			}
			else if (c == 127)
			{ // backspace
				if (pt > 0)
				{
					if (inp[pt - 1] == 9)
					{
						for (int i = 0; i < 7; i++)
						{
							printf("\b");
						}
					}
					inp[--pt] = '\0';
					printf("\b \b");
				}
			}
			else if (c == 9)
			{ // TAB character
				char orig[300];
				strcpy(orig, inp);
				// printf("%s\n",inp);
				// printf("h\n%s\n",inp);
				inp[pt++] = c;
				for (int i = 0; i < 8; i++)
				{ // TABS should be 8 spaces
					printf(" ");
				}
				inp[pt - 1] = '\0';
				inp[pt--];
				int ct = 0;
				char *pathauto = NULL;
				char *token = strtok(inp, " \t\n");
				while (token != NULL)
				{
					// printf("%d\n",ct);
					if (ct > 0)
						pathauto = token;
					ct++;
					token = strtok(NULL, " \t\n");
				}
				if (pathauto == NULL)
				{
					// printf("hello\n%s\nhello", inp);

					// printf("kk\n");
					strcpy(extra_string, "");
					recomendations();
					shell_prompt(home_path);
					printf("%s", orig);
					strcpy(inp, orig);
					printf("%s", &add_str[strlen(extra_string)]);
					continue;
					// printf("%s", &add_str[strlen(extra_string)]);
					// return inp;
				}
				char *token2 = strtok(pathauto, " \t\n");
				// printf("o\n");
				char pte[200];
				getcwd(pte, sizeof(pte));
				// printf("%s\n",token2);
				if (token2 == NULL)
				{
					// printf("hello\n%s\nhello", inp);
					strcpy(extra_string, "");
					recomendations();
					shell_prompt(home_path);
					printf("%s", orig);
					strcpy(inp, orig);
					printf("%s", &add_str[strlen(extra_string)]);
					continue;
					// printf("%s", &add_str[strlen(extra_string)]);
					// return inp;
				}
				// printf("%s\n",token2);
				int sp = 0, ep = strlen(token2) - 1;
				// printf("%c\n", token2[sp]);
				// printf("%c\n", token2[ep]);
				while ((token2[sp] == ' ' || token2[sp] == '\t') && sp < strlen(token2))
					sp++;
				while (ep >= 0 && ((token2[ep] == ' ' || token2[ep] == '\t') || token2[ep] != '/'))
					ep--;
				// sp = 0;
				int shell_flag = -1;
				if (ep == -1)
					ep = 0;
				if (token2[ep] == '/')
				{
					strcpy(extra_string, &token2[ep + 1]);
				}
				else
					strcpy(extra_string, &token2[ep]);

				token2[ep] = '\0';
				printf("\n");
				if (token2[sp] == '/')
				{
					shell_flag = 5;
					shell_prakat_ho(shell_flag, &token2[sp]);
				}
				else if (strcmp(&token2[sp], "..") == 0)
				{
					shell_flag = 1;
					shell_prakat_ho(shell_flag, "");
				}
				else if (strcmp(&token2[sp], ".") == 0)
				{
					shell_flag = 0;
					shell_prakat_ho(shell_flag, "");
				}
				else if (strcmp(&token2[sp], "-") == 0)
				{
					shell_flag = 3;
					shell_prakat_ho(shell_flag, "");
				}
				else if (token2[sp] == '~')
				{
					shell_flag = 4;
					shell_prakat_ho(shell_flag, &token2[sp + 1]);
				}
				else
				{
					shell_flag = 2;
					shell_prakat_ho(shell_flag, token2);
				}
				recomendations();
				chdir(pte);
				// printf("hello\n%s\nhello", inp);
				// printf("\n%s\n",orig);
				strcat(orig, &add_str[strlen(extra_string)]);
				shell_prompt(home_path);
				printf("%s", orig);
				pt += strlen(&add_str[strlen(extra_string)]);
				strcpy(inp, orig);
			}
			else if (c == 4)
			{
				exit(0);
			}
			else
			{
				printf("%d\n", c);
			}
		}
		else
		{
			inp[pt++] = c;
			printf("%c", c);
		}
	}
	disableRawMode();
	// printf("ok\n");
	// printf("hello%s\n",inp);
	return inp;
	// printf("\nInput Read: [%s]\n", inp);

	// }
}

int main()
{
	char *token_for_semicolon;
	char inst[100][10005];
	char input[10005];
	int total_inst = 0;
	int prompt_flag = 0;
	getcwd(home_path, sizeof(home_path));

	FILE *hist_file = fopen("history.txt", "r");
	int ed = 0;
	if (hist_file == NULL)
	{
		hist_file = fopen("history.txt", "w");
	}
	else
	{
		while (fscanf(hist_file, "%s ", history[ed]) == 1)
		{
			ed++;
		}
	}
	fclose(hist_file);
	int hist_iter = ed;

	while (1)
	{
		signal(SIGCHLD, handler_sigchld);
		signal(SIGINT, ctrlc_handler);
		signal(SIGTSTP, ctrlz_handler);

		total_inst = 0;
		shell_prompt(home_path);
		// strcpy(add_str, "");
		// strcpy(init_string, "");

		// fgets(input,10004,stdin);
		good = takeInput();
		strcpy(boy, good);
		printf("\n");
		// printf("\n");
		// printf("%s\n",good);
		strcpy(input, good);

		int hist_flag = 0;
		int h_flag = 0;
		for (int i = 0; i < strlen(input); i++)
		{
			if (input[i] != '\t' && input[i] != ' ' && input[i] != '\n')
			{
				h_flag = 1;
				break;
			}
		}

		if (h_flag == 1 && hist_iter > 0 && strcmp(history[hist_iter - 1], input) != 0)
		{
			if (hist_iter < 20)
			{
				strcpy(history[hist_iter], input);
				hist_iter++;
			}
			else
			{
				for (int i = 1; i < 20; i++)
				{
					strcpy(history[i - 1], history[i]);
				}
				strcpy(history[19], input);
			}
		}
		else if (h_flag == 1 && strcmp(history[hist_iter - 1], input) != 0)
		{
			strcpy(history[0], input);
			hist_iter++;
		}
		token_for_semicolon = strtok(input, ";");
		// printf("%s\n",token_for_semicolon);
		prompt_flag = 0;
		while (token_for_semicolon != NULL)
		{
			for (int i = 0; i < strlen(token_for_semicolon); i++)
			{
				if (token_for_semicolon[i] != '\t' && token_for_semicolon[i] != ' ' && token_for_semicolon[i] != '\n')
				{
					prompt_flag = 1;
					break;
				}
			}
			if (prompt_flag == 0)
				break;
			strcpy(inst[total_inst], token_for_semicolon);
			// printf("%s\n",inst[total_inst]);
			total_inst++;
			token_for_semicolon = strtok(NULL, ";");
		}
		char *token_for_inst;
		int in_iter = 0;
		char *ch;
		int ans_parse = 0;
		int check_bg[300] = {0};
		char tempo[200];
		char inst_set[100][1000];
		int i_set = 0;
		for (int i = 0; i < total_inst; i++)
		{
			ans_parse = 0;
			prompt_flag = 0;
			int st = 0;
			for (int j = 0; j < strlen(inst[i]); j++)
			{
				if (inst[i][j] == '&')
				{
					check_bg[i_set] = 1;
					tempo[ans_parse] = '\0';
					for (int i = 0; i < strlen(tempo); i++)
					{
						if (tempo[i] != '\t' && tempo[i] != ' ' && tempo[i] != '\n')
						{
							prompt_flag = 1;
							st = i;
							break;
						}
					}
					if (prompt_flag == 1)
					{
						strcpy(inst_set[i_set], &tempo[st]);
						i_set++;
					}
					ans_parse = 0;
				}
				else
				{
					tempo[ans_parse++] = inst[i][j];
				}
			}
			tempo[ans_parse] = '\0';
			for (int i = 0; i < strlen(tempo); i++)
			{
				if (tempo[i] != '\t' && tempo[i] != ' ' && tempo[i] != '\n')
				{
					prompt_flag = 1;
					break;
				}
			}
			if (prompt_flag == 1)
			{
				strcpy(inst_set[i_set], tempo);
				i_set++;
			}
		}

		while (in_iter < i_set)
		{
			char **pipelined_inst = (char **)malloc(100 * sizeof(char *));
			int flag_pipe = 0;
			int itr = 0;
			int pp = 0;

			for (int i = 0; i < 100; i++)
			{
				pipelined_inst[i] = (char *)malloc(1000 * sizeof(char));
			}
			for (int i = 0; i < strlen(inst_set[in_iter]); i++)
			{
				if (inst_set[in_iter][i] == '|')
				{
					itr = 0;
					pp++;
					flag_pipe = 1;
				}
				else
				{
					pipelined_inst[pp][itr++] = inst_set[in_iter][i];
				}
			}
			itr = 0;
			int f, stdi = 0, stdo = 1;
			for (int j = 0; j < pp + 1; j++)
			{
				int redirect = -1;
				char input_file[100] = "";
				char output_file[100] = "";
				int store = strlen(inst_set[in_iter]);
				char append_of[100] = "";
				for (int i = 0; i < strlen(pipelined_inst[j]); i++)
				{
					if (redirect == 0)
					{
						input_file[itr++] = pipelined_inst[j][i];
					}
					if (redirect == 1)
					{
						output_file[itr++] = pipelined_inst[j][i];
					}
					if (redirect == 2)
					{
						append_of[itr++] = pipelined_inst[j][i];
					}
					if (i + 1 < strlen(pipelined_inst[j]) && pipelined_inst[j][i] == '>' && pipelined_inst[j][i + 1] == '>')
					{
						if (i < store)
							store = i;
						itr = 0;
						redirect = 2;
						i++;
					}
					else if (pipelined_inst[j][i] == '<')
					{
						if (i < store)
							store = i;
						itr = 0;
						redirect = 0;
					}
					else if (pipelined_inst[j][i] == '>')
					{
						if (i < store)
							store = i;
						itr = 0;
						redirect = 1;
					}
				}
				stdo = 1, stdi = 0;
				if (strlen(output_file) > 0)
				{
					stdo = dup(1);
					char *ofname = strtok(output_file, " \t\n");
					f = open(ofname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					int status = dup2(f, STDOUT_FILENO);
					close(f);
					while (ofname != NULL)
					{
						ofname = strtok(NULL, " \t\n");
					}
				}
				else if (strlen(append_of) > 0)
				{
					stdo = dup(1);
					char *ofname = strtok(append_of, " \t\n");
					f = open(ofname, O_APPEND | O_CREAT | O_WRONLY, 0644);
					int status = dup2(f, STDOUT_FILENO);
					close(f);
					while (ofname != NULL)
					{
						ofname = strtok(NULL, " \t\n");
					}
				}
				if (flag_pipe == 0)
				{
					inst_set[in_iter][store] = '\0';
				}
				char *ofname;
				pipelined_inst[j][store] = '\0';
				if (strlen(input_file) > 0)
				{
					stdi = dup(0);
					ofname = strtok(input_file, " \t\n");
					char lite[100];
					if (flag_pipe == 0)
					{
						strcpy(lite, ofname);
						strcat(inst_set[in_iter], " ");
						strcat(inst_set[in_iter], lite);
					}
					strcpy(lite, ofname);
					strcat(pipelined_inst[j], " ");
					strcat(pipelined_inst[j], lite);
					while (ofname != NULL)
					{
						ofname = strtok(NULL, " \t\n");
					}
				}
			}

			if (flag_pipe == 1)
			{
				pipe_fg(pipelined_inst, pp, hist_iter);
			}
			else if (check_bg[in_iter] == 1)
			{
				strcpy(bg_name, inst_set[in_iter]);
				bg(inst_set[in_iter]);
				printf("[%d] %d\n", child_bg_process, bg_proccess[child_bg_process - 1].pid);
			}
			else
			{
				strcpy(fg_proccess[count_fg_process].pname, inst_set[in_iter]);
				fg_proccess[count_fg_process].pid = 0;
				count_fg_process++;
				token_for_inst = strtok(inst_set[in_iter], " \t\n");
				if (token_for_inst == NULL)
				{
					in_iter++;
					continue;
				}
				fg(token_for_inst, hist_iter);
			}
			char nw[1000];
			strcpy(nw, home_path);
			strcat(nw, "/history.txt");
			hist_file = fopen(nw, "w");
			for (int i = 0; i < hist_iter; i++)
			{
				fprintf(hist_file, "%s\n", history[i]);
			}

			fclose(hist_file);
			dup2(stdi, 0);
			dup2(stdo, 1);
			in_iter++;
		}
		strcpy(input, "");
	}
	return 0;
}