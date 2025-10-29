#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WORDS	"/usr/share/dict/words"

static char *
randomLine(char *s, int size, FILE *fp, int nLines)
{
	int i, rnd;

	rewind(fp);
	rnd = random() % nLines;
	for (i = 0; i <= rnd; i++)
		if (fgets(s, size, fp) == NULL)
			return NULL;

	return s;
}

int
main(int argc, char *argv[])
{
	FILE *fp0, *fp;
	int ccur, dcur, i, j, nhcur[5], nWords;
	char check[256], cset[256], dset[256], word[256];
	char notHere[5][256];
	char pattern[] = ".....";
	char *p;

	srandom((unsigned)time(NULL));

	/* 辞書ファイルを開く */
	fp0 = fopen(WORDS, "r");
	if (fp0 == NULL)
		err(1, "%s", WORDS);

	/* 一時ファイルを開く */
	fp = tmpfile();
	if (fp == NULL)
		err(1, "tmpfile");

	/* 小文字の五文字の語を一時ファイルに憶える */
	nWords = 0;
	while (errno = 0, fgets(word, sizeof(word), fp0) != NULL) {
		*strchr(word, '\n') = '\0';
		/* 五文字でない語に興味はない */
		if (strlen(word) != 5)
			goto next;
		/* 小文字でない語に興味はない */
		for (i = 0; i < 5; i++)
			if (!islower(word[i]))
				goto next;
		/* 一時ファイルに憶える */
		(void)strncat(word, "\n", sizeof(word)-1);
		if (fputs(word, fp) == EOF)
			err(1, "fputs");
		nWords++;
next:
		;
	}
	if (errno != 0)
		err(1, "%s", WORDS);


	ccur = 0;
	(void)memset(cset, 0, sizeof(cset));
	dcur = 0;
	(void)memset(dset, 0, sizeof(dset));
	(void)memset(nhcur, 0, sizeof(nhcur));
	(void)memset(notHere, 0, sizeof(notHere));
	for (i = 0; i < 6; i++) {
		/* 辞書ファイルにもう用はない */
		(void)fclose(fp0);
		fp0 = fp;

		/* ひとまず適当に語を表示しておく */
		if (randomLine(word, sizeof(word), fp0, nWords) == NULL)
			err(1, "randomLine");
		*strchr(word, '\n') = '\0';
		(void)printf("%d -> %s\n", nWords, word);
		if (randomLine(word, sizeof(word), fp0, nWords) == NULL)
			err(1, "randomLine");
		*strchr(word, '\n') = '\0';
		(void)printf("%d -> %s\n", nWords, word);
		if (randomLine(word, sizeof(word), fp0, nWords) == NULL)
			err(1, "randomLine");
		*strchr(word, '\n') = '\0';
		(void)printf("%d -> %s\n", nWords, word);

		(void)scanf("%s", check);
		for (j = 0; j < 5; j++) {
			/* 灰色の文字 */
			if (check[j] == 'x')
				dset[dcur++] = word[j];
			/* 黄色の文字 */
			if (check[j] == 'y') {
				cset[ccur++] = word[j];
				notHere[j][nhcur[j]++] = word[j];
			}
			/* 緑色の文字 */
			if (check[j] == 'o') {
				cset[ccur++] = word[j];
				pattern[j] = word[j];
			}
		}

		for (j = 0; cset[j] != '\0'; j++)
			while ((p = strchr(dset, cset[j])) != NULL)
				*p = '.';

		/* 一時ファイルを開く */
		fp = tmpfile();
		if (fp == NULL)
			err(1, "tmpfile");

		/* 条件を満たす語を一時ファイルに憶える */
		nWords = 0;
		rewind(fp0);
		while (errno = 0, fgets(word, sizeof(word), fp0) != NULL) {
			*strchr(word, '\n') = '\0';

			/* 灰色の文字を含んでいる */
			if (strlen(dset) > 0 && strpbrk(word, dset) != NULL)
					goto next2;

			/* 黄色の文字を含んでいない */
			for (j = 0; cset[j] != '\0'; j++)
				if (strchr(word, cset[j]) == NULL)
					goto next2;

			/* 黄色の文字を忌避位置で含んでいる */
			for (j = 0; j < 5; j++) {
				if (strlen(notHere[j]) > 0 && strchr(notHere[j], word[j]) != NULL)
					goto next2;
			}

			/* 緑色の文字をその位置に含んでいない */
			for (j = 0; j < 5; j++) {
				if (pattern[j] != '.' && pattern[j] != word[j])
					goto next2;
			}

			/* 一時ファイルに憶える */
			(void)strncat(word, "\n", sizeof(word)-1);
			if (fputs(word, fp) == EOF)
				err(1, "fputs");
			nWords++;
next2:
			;
		}
		if (errno != 0)
			err(1, "%s", WORDS);
	}
}
