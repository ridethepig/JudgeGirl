#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ptrace.h>
#include <sys/reg.h>

#include "exit_code.h"
#include "syscall_listener.h"
#include "common.h"

#ifdef __x86_64__

int call_count[512]={0};

void listen_me();
void listen_again();
int parse_status(int);
void check_call(pid_t);

const int syscall_limit[]=
{
 0,  0, -1, 
-1, -1,  0, 
-1, -1,  0, 
 0, -1,  0, 
 0,  0, -1, 
//1
-1,  0,  0, 
 0,  0,  0, 
 0, -1, -1, 
-1,  0, -1, 
-1, -1, -1, 
//2
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//3
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1,  1,  //attention here is 1
//4
 0, -1, -1, 
 0, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//5
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1,  0, 
//6
-1, -1, -1, 
-1, -1, -1, 
 0,  0, -1, 
-1, -1, -1, 
-1, -1, -1, 
//7
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//8
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1,  0, 
//9
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//10
-1, -1, -1, 
-1, -1, -1, 
-1, -1,  0, 
-1, -1, -1, 
-1, -1, -1, 
//11
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//12
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//13
-1, -1, -1, 
-1, -1, -1, 
 0, -1, -1, 
-1,  0, -1, 
-1, -1, -1, 
//14
-1,  0, -1, 
-1, -1, -1, 
-1, -1, -1, 
 0, -1, -1, 
-1, -1, -1, 
//15
-1, -1, -1, 
-1, -1, -1, 
 0, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//16
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//17
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//18
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//19
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1,  0,  0, 
-1, -1, -1, 
//20
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
//21
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, -1, 
-1, -1, 
};

const char* const syscall_list[]= 
{
"sys_read", "sys_write", "sys_open", 
"sys_close", "sys_stat", "sys_fstat", 
"sys_lstat", "sys_poll", "sys_lseek", 
"sys_mmap", "sys_mprotect", "sys_munmap", 
"sys_brk", "sys_rt_sigaction", "sys_rt_sigprocmask", 
//1
"sys_rt_sigreturn", "sys_ioctl", "sys_pread64", 
"sys_pwrite64", "sys_readv", "sys_writev", 
"sys_access", "sys_pipe", "sys_select", 
"sys_sched_yield", "sys_mremap", "sys_msync", 
"sys_mincore", "sys_madvise", "sys_shmget", 
//2
"sys_shmat", "sys_shmctl", "sys_dup", 
"sys_dup2", "sys_pause", "sys_nanosleep", 
"sys_getitimer", "sys_alarm", "sys_setitimer", 
"sys_getpid", "sys_sendfile", "sys_socket", 
"sys_connect", "sys_accept", "sys_sendto", 
//3
"sys_recvfrom", "sys_sendmsg", "sys_recvmsg", 
"sys_shutdown", "sys_bind", "sys_listen", 
"sys_getsockname", "sys_getpeername", "sys_socketpair", 
"sys_setsockopt", "sys_getsockopt", "sys_clone", 
"sys_fork", "sys_vfork", "sys_execve", 
//4
"sys_exit", "sys_wait4", "sys_kill", 
"sys_uname", "sys_semget", "sys_semop", 
"sys_semctl", "sys_shmdt", "sys_msgget", 
"sys_msgsnd", "sys_msgrcv", "sys_msgctl", 
"sys_fcntl", "sys_flock", "sys_fsync", 
//5
"sys_fdatasync", "sys_truncate", "sys_ftruncate", 
"sys_getdents", "sys_getcwd", "sys_chdir", 
"sys_fchdir", "sys_rename", "sys_mkdir", 
"sys_rmdir", "sys_creat", "sys_link", 
"sys_unlink", "sys_symlink", "sys_readlink", 
//6
"sys_chmod", "sys_fchmod", "sys_chown", 
"sys_fchown", "sys_lchown", "sys_umask", 
"sys_gettimeofday", "sys_getrlimit", "sys_getrusage", 
"sys_sysinfo", "sys_times", "sys_ptrace", 
"sys_getuid", "sys_syslog", "sys_getgid", 
//7
"sys_setuid", "sys_setgid", "sys_geteuid", 
"sys_getegid", "sys_setpgid", "sys_getppid", 
"sys_getpgrp", "sys_setsid", "sys_setreuid", 
"sys_setregid", "sys_getgroups", "sys_setgroups", 
"sys_setresuid", "sys_getresuid", "sys_setresgid", 
//8
"sys_getresgid", "sys_getpgid", "sys_setfsuid", 
"sys_setfsgid", "sys_getsid", "sys_capget", 
"sys_capset", "sys_rt_sigpending", "sys_rt_sigtimedwait", 
"sys_rt_sigqueueinfo", "sys_rt_sigsuspend", "sys_sigaltstack", 
"sys_utime", "sys_mknod", "sys_uselib", 
//9
"sys_personality", "sys_ustat", "sys_statfs", 
"sys_fstatfs", "sys_sysfs", "sys_getpriority", 
"sys_setpriority", "sys_sched_setparam", "sys_sched_getparam", 
"sys_sched_setscheduler", "sys_sched_getscheduler", "sys_sched_get_priority_max", 
"sys_sched_get_priority_min", "sys_sched_rr_get_interval", "sys_mlock", 
//10
"sys_munlock", "sys_mlockall", "sys_munlockall", 
"sys_vhangup", "sys_modify_ldt", "sys_pivot_root", 
"sys__sysctl", "sys_prctl", "sys_arch_prctl", 
"sys_adjtimex", "sys_setrlimit", "sys_chroot", 
"sys_sync", "sys_acct", "sys_settimeofday", 
//11
"sys_mount", "sys_umount2", "sys_swapon", 
"sys_swapoff", "sys_reboot", "sys_sethostname", 
"sys_setdomainname", "sys_iopl", "sys_ioperm", 
"sys_create_module", "sys_init_module", "sys_delete_module", 
"sys_get_kernel_syms", "sys_query_module", "sys_quotactl", 
//12
"sys_nfsservctl", "sys_getpmsg", "sys_putpmsg", 
"sys_afs_syscall", "sys_tuxcall", "sys_security", 
"sys_gettid", "sys_readahead", "sys_setxattr", 
"sys_lsetxattr", "sys_fsetxattr", "sys_getxattr", 
"sys_lgetxattr", "sys_fgetxattr", "sys_listxattr", 
//13
"sys_llistxattr", "sys_flistxattr", "sys_removexattr", 
"sys_lremovexattr", "sys_fremovexattr", "sys_tkill", 
"sys_time", "sys_futex", "sys_sched_setaffinity", 
"sys_sched_getaffinity", "sys_set_thread_area", "sys_io_setup", 
"sys_io_destroy", "sys_io_getevents", "sys_io_submit", 
//14
"sys_io_cancel", "sys_get_thread_area", "sys_lookup_dcookie", 
"sys_epoll_create", "sys_epoll_ctl_old", "sys_epoll_wait_old", 
"sys_remap_file_pages", "sys_getdents64", "sys_set_tid_address", 
"sys_restart_syscall", "sys_semtimedop", "sys_fadvise64", 
"sys_timer_create", "sys_timer_settime", "sys_timer_gettime", 
//15
"sys_timer_getoverrun", "sys_timer_delete", "sys_clock_settime", 
"sys_clock_gettime", "sys_clock_getres", "sys_clock_nanosleep", 
"sys_exit_group", "sys_epoll_wait", "sys_epoll_ctl", 
"sys_tgkill", "sys_utimes", "sys_vserver", 
"sys_mbind", "sys_set_mempolicy", "sys_get_mempolicy", 
//16
"sys_mq_open", "sys_mq_unlink", "sys_mq_timedsend", 
"sys_mq_timedreceive", "sys_mq_notify", "sys_mq_getsetattr", 
"sys_kexec_load", "sys_waitid", "sys_add_key", 
"sys_request_key", "sys_keyctl", "sys_ioprio_set", 
"sys_ioprio_get", "sys_inotify_init", "sys_inotify_add_watch", 
//17
"sys_inotify_rm_watch", "sys_migrate_pages", "sys_openat", 
"sys_mkdirat", "sys_mknodat", "sys_fchownat", 
"sys_futimesat", "sys_newfstatat", "sys_unlinkat", 
"sys_renameat", "sys_linkat", "sys_symlinkat", 
"sys_readlinkat", "sys_fchmodat", "sys_faccessat", 
//18
"sys_pselect6", "sys_ppoll", "sys_unshare", 
"sys_set_robust_list", "sys_get_robust_list", "sys_splice", 
"sys_tee", "sys_sync_file_range", "sys_vmsplice", 
"sys_move_pages", "sys_utimensat", "sys_epoll_pwait", 
"sys_signalfd", "sys_timerfd_create", "sys_eventfd", 
//19
"sys_fallocate", "sys_timerfd_settime", "sys_timerfd_gettime", 
"sys_accept4", "sys_signalfd4", "sys_eventfd2", 
"sys_epoll_create1", "sys_dup3", "sys_pipe2", 
"sys_inotify_init1", "sys_preadv", "sys_pwritev", 
"sys_rt_tgsigqueueinfo", "sys_perf_event_open", "sys_recvmmsg", 
//20
"sys_fanotify_init", "sys_fanotify_mark", "sys_prlimit64", 
"sys_name_to_handle_at", "sys_open_by_handle_at", "sys_clock_adjtime", 
"sys_syncfs", "sys_sendmmsg", "sys_setns", 
"sys_getcpu", "sys_process_vm_readv", "sys_process_vm_writev", 
"sys_kcmp", "sys_finit_module", "sys_sched_setattr", 
//21
"sys_sched_getattr", "sys_renameat2", "sys_seccomp", 
"sys_getrandom", "sys_memfd_create", "sys_kexec_file_load", 
"sys_bpf", "stub_execveat", "userfaultfd", 
"membarrier", "mlock2", "copy_file_range", 
"preadv2", "pwritev2"
};
void listen_me()
{
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL)==-1)
		error(EX_INTER, 0, "Error initiating ptrace");
}

void listen_again(pid_t child_pid)
{
	if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL)==-1)
		error(EX_INTER, 0, "Error ptracsing child");
}


void check_call(pid_t child_pid)
{
	int id = ptrace(PTRACE_PEEKUSER, child_pid, (void*)(8*ORIG_RAX), NULL);
	if (id==-1)
		error(EX_INTER, 0, "Error peeking user");
	call_count[id]++; 
	if ((call_count[id]&1) == 0)
		return;

	fprintf(f_log, "Calling   %-16s\t(id:%-4d)\t\n", syscall_list[id], id);
	if (syscall_limit[id] == 0) //no limits
		return;
	if (call_count[id] > syscall_limit[id])
	{
		ptrace(PTRACE_KILL, child_pid, NULL, NULL);        
		error(EX_INTER, 0, "forbidden operation");
	}
}

int parse_status(int status)
{
	if (WIFEXITED(status)) //exited
	{
		if (WEXITSTATUS(status) == 0)
			return EX_SUCCESS;
		else
			return EX_RE;
	}

	if (WIFSIGNALED(status))
		return EX_FATAL;

	if (WIFSTOPPED(status))
	{
		int sig = WSTOPSIG(status);
		switch (sig)
		{
			case SIGUSR1:         //regular check
				break;       //just a place holder
			case SIGTRAP:         //ptrace got an syscall (to call or has returned)
				return EX_YOYOCHECKNOW;
			case SIGXFSZ:         //write too much to file
				return EX_RE;//actually, it should be EX_OLE
			default:
				return EX_RE;
		}
	}
	return EX_NOTEND;
}

#endif