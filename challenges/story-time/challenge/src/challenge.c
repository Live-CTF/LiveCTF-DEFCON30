#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define STORY_SIZE 0x100

struct STORY {
	uint32_t size;
	char text[STORY_SIZE];
	void* decompress;
};


struct STORY story;


void compress(char* input, uint32_t inputLen, char* output, uint32_t* output_size)
{
	uint8_t counter = 0;
	char lastChar = 0;
	uint32_t output_idx = 0;

	for (uint32_t i = 0; i < inputLen; i++)
	{
		if (counter == 0)
		{
			lastChar = input[i];
			counter = 1;
			continue;
		}

		if (input[i] == lastChar && counter < 255)
		{
			counter += 1;
		}
		else
		{
			output[output_idx++] = counter;
			output[output_idx++] = lastChar;
			counter = 0;
			i -= 1;
			continue;
		}
	}

	if (counter > 0)
	{
		output[output_idx++] = counter;
		output[output_idx++] = lastChar;
	}
	*output_size = output_idx;
}


char* decompress(char* input, size_t inputLen)
{
	size_t outputLen = 1; // 1 for null byte
	for (size_t i = 0; i < inputLen-1; i+=2)
	{
		outputLen += input[i];
	}

	char* output = calloc(outputLen, 1);

	size_t written = 0;
	for(size_t i = 0; i < inputLen-1; i+=2)
	{
		uint8_t outputCount = input[i];
		char outputChar = input[i+1];

		for (uint8_t c = 0; c < outputCount; c++)
		{
			output[written++] = outputChar;
			if (written > outputLen - 1)
				return NULL;
		}
	}
	output[written] = 0;
	return output;
}


void print_menu()
{
	puts( \
"Welcome to Story Time! I am but a simple story teller, here to see what stories\n" \
"can be learned. Gather round the fire, and I will regale you with tales of yore.\n" \
"\n" \
"1) Tell me your story\n" \
"2) Hear your story\n" \
"3) End all stories\n\n" \
"Choice: ");
}


void create_story()
{
	char* input = malloc(STORY_SIZE);

	puts("Tell me your story:");
	if (!fgets(input, STORY_SIZE, stdin))
		return;

	input[strcspn(input, "\n")] = 0;

	size_t inputLen = strlen(input);

	if (inputLen < 0x7f)
	{
		memcpy(story.text, input, strlen(input));
	}
	else
	{
		story.decompress = &decompress;
		compress(input, inputLen, story.text, &story.size);
	}
}


void read_story()
{
	char* story_text = NULL;
	if (story.decompress != NULL)
	{
		char* (*decompress)(char*, uint32_t) = story.decompress;
		story_text = decompress(story.text, story.size);
	}
	else
	{
		story_text = story.text;
	}
	printf("Your story: %s\n", story_text);
}


void init()
{
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&story, 0, sizeof(story));
}


int main()
{
	init();
	char choice[0x10] = { 0 };

	while (1)
	{
		print_menu();
		if (!fgets(choice, sizeof(choice), stdin))
			break;

		switch(choice[0])
		{
			case '1':
				create_story();
				break;
			case '2':
				read_story();
				break;
			case '3':
				puts("That was a little dark, but ok.\n\n");
				exit(0);
			default:
				continue;
		}
	}

	return 0;
}


void win()
{
	system("/bin/sh");
}
