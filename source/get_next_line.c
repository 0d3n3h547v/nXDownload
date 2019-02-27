#include "includes/get_next_line.h"

static char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	int		i;

	if (s1 == NULL && s2 == NULL)
		return (NULL);
	else if (s1 == NULL)
		return (strdup(s2));
	else if (s2 == NULL)
		return (strdup(s1));
	i = strlen(s1) + strlen(s2) + 1;
	str = (char *)malloc(sizeof(char) * i);
	if (str == NULL)
		return (NULL);
	strcpy(str, s1);
	strcat(str, s2);
	return (str);
}

static char	*ft_strsub(char const *s, unsigned int start, size_t len)
{
	char	*str;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (s == NULL)
		return (NULL);
	str = calloc(sizeof(char), len+1);
	if (!str)
		return (NULL);
	while (i < start)
		i++;
	while (j < len)
	{
		str[j] = s[i];
		i++;
		j++;
	}
	return (str);
}

static int	split_stat(char **line, char **stat)
{
	char	*content2 = NULL;
	char	*tmp = NULL;
	int		len;

	if (!(*stat))
		return (0);
	if ((content2 = strchr(*stat, '\n'))) {
		len = content2 - *stat;
		*line = ft_strsub(*stat, 0, len);

		if (strlen(++content2)) {
			tmp = *stat;
			*stat = strdup(content2);
			free(tmp);
		} else {
			free(*stat);
			*stat = NULL;
		}
	}
	else if (strlen(*stat)) {
		*line = *stat;
		*stat = NULL;
	}
	return (content2 ? 1 : 0);
}

static int	read_file(char *buf, char **line, char **stat)
{
	char	*content;
	char	*tmp;

	if ((content = strchr(buf, '\n')))
	{
		content[0] = '\0';
		tmp = ft_strjoin(*line, buf);
		free(*line);
		*line = tmp;
		if (strlen(++content))
			*stat = strdup(content);
	}
	else
	{
		tmp = ft_strjoin(*line, buf);
		free(*line);
		*line = tmp;
	}
	return (content ? 1 : 0);
}

int			get_next_line(int const fd, char **line)
{
	static char	*stat = NULL;
	char		*buf;
	int			ret;

	if (fd < 0 || line == NULL || BUFF_SIZE <= 0 || fd > 256)
		return (-1);
	*line = NULL;
	buf = calloc(sizeof(char), BUFF_SIZE +1);
	if (split_stat(line, &stat) == 1)
	{
		free(buf);
		return (1);
	}
	while ((ret = read(fd, buf, BUFF_SIZE)) > 0)
	{
		buf[ret] = '\0';
		if (read_file(buf, line, &stat) == 1)
			break ;
	}
	free(buf);
	if (ret > 0 || *line)
		return (1);
	return (ret);
}
