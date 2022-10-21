// FILE UPLOAD EXAMPLE
// ------WebKitFormBoundaryEanrNle4vGeLI1lm
// Content-Disposition: form-data; name="lname"; filename="to_upload.txt"
// Content-Type: text/plain

// hello world

// ------WebKitFormBoundaryEanrNle4vGeLI1lm--

#ifndef UPFILE_HPP
#define UPFILE_HPP

#include <iostream>
#include <map>

namespace webserv
{
	class UpFile
	{
		private:
			std::string							_delimiter;
			std::string							_fileName;
			std::string							_fileContent;
			std::string							_buffer;
			std::map<std::string, std::string>	_files;

		public:
			UpFile();
			UpFile(UpFile const &other);
			~UpFile();
			UpFile& operator=(UpFile const &other);

			std::string const	&get_delimiter(void) const;
			std::string const	&get_fileName(void) const;
			std::string const	&get_fileContent(void) const;

			void				set_delimiter(std::string delimiter);
			void				set_fileName(std::string fileName);
			void				set_fileContent(std::string fileContent);

			void				parse_fileStream(void);
			void				write_to_file(std::string const &path);
			void				append_buf(char *buf, size_t n);
	};
}

#endif
