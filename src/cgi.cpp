//
// cgi.cpp
//

#include <map>
#include <sys/types.h>
#include <sys/wait.h>
#include "utils.hpp"
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#define ENVP_COUNT_MAX 1024

void create_envp(char **envp, std::map<std::string, std::string> &map);
const char * get_value_of_key(std::map<std::string, std::string> &map, const char *key);
char *add_to_c_vector(char **vector, std::map<std::string, std::string> &map, const char *key);
int find_next_index(char **vector);
void free_c_vector(char **vector);

/*
the '&' character inside of the actual data gets translated to "%26"
'?' translates to "%3F"
	- also translated: ' '(space), '=', '%'?
*/
std::string run_cgi_script(std::map<std::string, std::string> envp_map,
	std::string request_body)
{
	const char *bin_file;
	const char *script_name;
	const char *method;
	char buffer[4096];
	char *envp[ENVP_COUNT_MAX] = { NULL };
	char *argv[3] = { NULL, NULL, NULL };
	int bytes_read;
	int fds[2];
	pid_t pid;
	std::string cgi_response;

	LOG_I() << "Running the CGI script.\n";
	if (pipe(fds) == -1)
		throw std::runtime_error("CGI Error - pipe() failed!");

	bin_file = get_value_of_key(envp_map, "PATH_INFO");
	if (bin_file == NULL)
		throw std::runtime_error("CGI Error - Bin file not found!");
	// LOG_D() << "Bin to run: " << bin_file << "\n";

	script_name = get_value_of_key(envp_map, "SCRIPT_NAME");
	if (script_name == NULL)
		throw std::runtime_error("CGI Error - Script name not found!");
	// LOG_D() << "Script to run: " << script_name << "\n";

	create_envp(envp, envp_map);
	argv[0] = (char *)bin_file;
	argv[1] = (char *)script_name;

	pid = fork();
	if (pid < 0)
	{
		free_c_vector(envp);
		throw std::runtime_error("CGI Error - fork() failed!");
	}
	else if (pid == 0) // child
	{
		method = envp_map.find("REQUEST_METHOD")->second.c_str();
		// LOG_D() << "REQUEST_METHOD=" << method << '\n';
		if (std::strcmp(method, "POST") == 0 && request_body.length() > 0)
		{
			FILE *file = std::fopen("tempfile_", "w");
			if (file == NULL)
				throw std::runtime_error("CGI Error - fopen() failed!");
			std::fwrite((char *) request_body.c_str(), sizeof(char), request_body.length() + 1, file);
			std::fclose(file);
			int fd = open("tempfile_", O_RDONLY); // !!!!! delete file
			unlink("tempfile_");
			fds[0] = fd;
			dup2(fds[0], STDIN_FILENO);
		}

		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);

		execve(bin_file, argv, envp);
		LOG_E() << "CGI Error - execve() failed.\n";
		throw std::runtime_error("CGI Error - execve() failed!");
	}
	else if (pid > 0) // parent
	{
		waitpid(pid, NULL, 0);
		close(fds[1]);

		do
		{
			bytes_read = read(fds[0], buffer, 4095);
			buffer[bytes_read] = '\0';
			cgi_response += buffer;
		}
		while (bytes_read != 0);

		close(fds[0]);
	}
	free_c_vector(envp);
	return cgi_response;
}

void create_envp(char **envp, std::map<std::string, std::string> &map)
{
	add_to_c_vector(envp, map, "SERVER_SOFTWARE");
	add_to_c_vector(envp, map, "SERVER_NAME");
	add_to_c_vector(envp, map, "GATEWAY_INTERFACE");

	add_to_c_vector(envp, map, "SERVER_PROTOCOL");
	add_to_c_vector(envp, map, "SERVER_PORT");
	add_to_c_vector(envp, map, "REQUEST_METHOD");
	add_to_c_vector(envp, map, "REQUEST_URI");
	add_to_c_vector(envp, map, "PATH_INFO");
	add_to_c_vector(envp, map, "PATH_TRANSLATED");
	add_to_c_vector(envp, map, "SCRIPT_NAME");
	add_to_c_vector(envp, map, "QUERY_STRING");
	add_to_c_vector(envp, map, "REMOTE_HOST");
	add_to_c_vector(envp, map, "REMOTE_ADDR");
	add_to_c_vector(envp, map, "AUTH_TYPE");
	add_to_c_vector(envp, map, "REMOTE_USER");
	add_to_c_vector(envp, map, "REMOTE_IDENT");
	add_to_c_vector(envp, map, "CONTENT_TYPE");
	add_to_c_vector(envp, map, "CONTENT_LENGTH");

	add_to_c_vector(envp, map, "HTTP_HOST");
	add_to_c_vector(envp, map, "HTTP_USER_AGENT");
	add_to_c_vector(envp, map, "HTTP_ACCEPT");
	add_to_c_vector(envp, map, "HTTP_ACCEPT_LANGUAGE");
	add_to_c_vector(envp, map, "HTTP_ENCODING");
	add_to_c_vector(envp, map, "HTTP_CONNECTION");
	add_to_c_vector(envp, map, "HTTP_UPGRADE_INSECURE_REQUESTS");
}

const char * get_value_of_key(std::map<std::string, std::string> &map, const char *key)
{
	std::map<std::string, std::string>::iterator it;

	it = map.find(key);
	if (it != map.end())
		return it->second.c_str();
	return NULL;
}

char *add_to_c_vector(char **vector, std::map<std::string, std::string> &map, const char *key)
{
	int index;
	std::map<std::string, std::string>::iterator it;
	std::string temp;

	it = map.find(key);
	if (it != map.end())
	{
		index = find_next_index(vector);
		if (index == -1)
			return NULL;

		temp = it->first + "=" + it->second;
		vector[index] = new char[temp.length() + 1];
		if (vector[index] == NULL)
			return NULL;
		std::memcpy(vector[index], temp.c_str(), temp.length());
		vector[index][temp.length()] = '\0';
		return vector[index];
	}
	return NULL;
}

int find_next_index(char **vector)
{
	int i = 0;

	while (vector[i] != NULL)
		i++;
	if (i == ENVP_COUNT_MAX - 1)
		return -1;
	return i;
}

void	free_c_vector(char **vector)
{
	for (int i = 0; vector[i] != NULL; i++)
	{
		delete[] vector[i];
		vector[i] = NULL;
	}
}
