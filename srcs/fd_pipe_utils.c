/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   piping.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vdragomi <vdragomi@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 14:56:51 by vdragomi          #+#    #+#             */
/*   Updated: 2021/12/14 14:56:51 by vdragomi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/pipex.h"

void	ft_pipex(int argc, char **argv, t_list *cmd_list, char **envp)
{
	int		fd[2];
	pid_t	pid;
	int		i;

	i = 0;
	ft_initialize_fds(fd, argc, argv, cmd_list);
	while (i < argc - 3 - 1)
	{
		if (pipe(fd) == -1)
			ft_exit_on_error(&cmd_list, "Pipe creation failed");
		pid = fork();
		if (pid == -1)
			ft_exit_on_error(&cmd_list, "Forking failed");
		else if (pid == 0)
			ft_execute_child_process(cmd_list, envp, fd);
		else
		{
			ft_execute_parent_process(pid, fd, &cmd_list);
			if (cmd_list->next != NULL)
				cmd_list = cmd_list->next;
			i++;
		}
		if (i == argc - 3 - 1)
			ft_execute_last_cmd(pid, cmd_list, envp);
	}
}

void	ft_initialize_fds(int *fd, int argc, char **argv, t_list *cmd_list)
{
	fd[0] = open(argv[1], O_RDONLY);
	if (fd[0] == -1)
		ft_exit_on_error(&cmd_list, "Error on opening input file");
	fd[1] = open(argv[argc -1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd[1] == -1)
		ft_exit_on_error(&cmd_list, "Error on opening output file");
	if (dup2(fd[0], STDIN_FILENO) == -1)
		ft_exit_on_error(&cmd_list, "File descriptor duplication failed");
	if (dup2(fd[1], STDOUT_FILENO) == -1)
		ft_exit_on_error(&cmd_list, "File descriptor duplication failed");
	if (close(fd[0]) == -1)
		ft_exit_on_error(&cmd_list, "Close of fd[0] failed");
	if (close(fd[1]) == -1)
		ft_exit_on_error(&cmd_list, "Close of fd[1] failed");
}

void	ft_execute_child_process(t_list *cmd_list, char **envp, int *fd)
{
	if (cmd_list->next != NULL)
		if (dup2(fd[1], STDOUT_FILENO) == -1)
			ft_exit_on_error(&cmd_list, "File descriptor duplication failed");
	if (close(fd[0]) == -1)
		ft_exit_on_error(&cmd_list, "Close of fd[0] failed");
	if (close(fd[1]) == -1)
		ft_exit_on_error(&cmd_list, "Close of fd[1] failed");
	if (execve(((t_content *)(cmd_list->content))->path, \
		((t_content *)(cmd_list->content))->cmd_n_flags, envp) == -1)
		ft_exit_on_error(&cmd_list, "Command execution failed");
}

void	ft_execute_parent_process(int pid, int *fd, t_list **cmd_list)
{
	waitpid(pid, NULL, 0);
	errno = 0;
	if (dup2(fd[0], STDIN_FILENO) == -1)
		ft_exit_on_error(cmd_list, "File descriptor duplication failed");
	if (close(fd[0]) == -1)
		ft_exit_on_error(cmd_list, "Close of fd[0] failed");
	if (close(fd[1]) == -1)
		ft_exit_on_error(cmd_list, "Close of fd[1] failed");
}

void	ft_execute_last_cmd(pid_t pid, t_list *cmd_list, char **envp)
{
	waitpid(pid, NULL, 0);
	if (execve(((t_content *)(cmd_list->content))->path, \
	((t_content *)(cmd_list->content))->cmd_n_flags, envp) == -1)
		ft_exit_on_error(&cmd_list, "Command execution failed");
}
