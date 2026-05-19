#include <stdio.h>			
#include "vendor/mlib/mfile.h"
#include "vendor/mlib/mvector.h"
char* string_new(MVecParamDefPtr(*string_pool, char*), const char* str) {
    return MVecPush(*string_pool, (char*)str);
}

#define MStringPool(name) MVecAllocDefault(name, char*)
#define MStringPoolPush(string_pool, string) string_new(MVecParamRefPtr(string_pool), (string))

static char tokens[] = { '#', '*', '-', '.', '|', '>'};

int main(int argc, char** argv) {
	MStringPool(html);
	if (argc < 2) {
		fprintf(stderr, "provdie a markdown\n");	
		return 1;
	}
	char* fileName = argv[1];
		
	Mstr* file_buffer = NULL;	
	catch(mfile_read, &file_buffer, fileName) {
		fprintf(stderr, "%s\n", unwrap_fail(file_buffer));
		return 1;
	};

	Mstr* token = NULL;
	for(;;) {
		catch(mstr_chop_by, &token, file_buffer, '\n') {
			fprintf(stderr, "Failed to chop md: %s\n", unwrap_fail(token));
			return 1;
		}
		if (token == NULL) break;
		mstr_trim(token);
		if (token->length == 0) continue;
		switch (token->raw[0]) {
		    case '#':
		        Mstr* content = NULL;
				mstr_chop_by_tryfail(&content, token, '#');
				MStringPoolPush(&html, MPRINT_FMT_OUT("<h1>"MFMT(token)"</h1>"));
		        break;
		    case '-':
				int count = 0;
				while (token->raw[count] == '-') count++;
				if (count == 3)
					MStringPoolPush(&html, MPRINT_FMT_OUT("<hr>"));
		        break;
		    case '!':
		    	Mstr* gbg = NULL;
		    	Mstr* image_alt = NULL;
		    	Mstr* image_url = NULL;
   				mstr_chop_by_tryfail(&gbg, token, '[');
   				mstr_chop_by_tryfail(&image_alt, token, ']');
   				mstr_chop_by_tryfail(&gbg, token, '(');
 				mstr_chop_by_tryfail(&image_url, token, ')');
   				MStringPoolPush(&html, MPRINT_FMT_OUT("<img alt='"MFMT(image_alt)"' src='"MFMT(image_url)"'/>"));
   		        break;
   		    case '*':
		    	Mstr* gbgb = NULL;
		    	Mstr* bold_content = NULL;
   		    	
	    		mstr_chop_by_tryfail(&gbgb, token, '*');
	    		mstr_chop_by_tryfail(&gbgb, token, '*');
				catch(mstr_chop_by, &bold_content, token, '*') break;
				catch(mstr_chop_by, &gbgb, token, '*') break;
				catch(mstr_chop_by, &gbgb, token, '*') break;
				MStringPoolPush(&html, MPRINT_FMT_OUT("<b>"MFMT(bold_content)"</b>"));
  		        break;
		    default:
		        MStringPoolPush(&html, MPRINT_FMT_OUT("<p>"MFMT(token)"</p>"));
		}
	}

	char* out = NULL;
	char* path = MPRINT_FMT_OUT("./"$(fileName)".html");
	char* htmlContent = NULL;
	MVecForeach(item, html) {
		htmlContent = MPRINT_FMT_OUT($(htmlContent ? htmlContent : "")$(item));
		free(item);
	}
	catch(mfile_create, &out, path, htmlContent) {
		fprintf(stderr, "Error wrinting html: %s\n", unwrap_fail(out));
		free(out);
		return 1;
	};

	free(MVec(html));
	free(path);
	free(file_buffer);
	return 0;				
}							
