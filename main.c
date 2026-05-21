#include <stdio.h>			
#include <stdbool.h>
#include "vendor/mlib/mfile.h"
#include "vendor/mlib/mvector.h"
#include "vendor/mlib/mprint.h"

static char tokens[] = { '#', '*', '-', '.', '|', '>'};
int main(int argc, char** argv) {
	__free(strfree) MVecAlloc(pool, char, 500);
	__free(strfree) MVecAlloc(html, char, 500);
	if (argc < 2 || argv[1] == NULL) {
		MPrintFmt("provdie a markdown");
		return 1;
	}
	
	MstrView fileName = MStrFmt(&pool, $(argv[1]));
	MRetEither(mk, mkerr, mfile_read(MVecParamRefPtr(&pool), fileName));
	if (mkerr) {
		MPrintFmt("File read fail: "$(mkerr));
		return 1;
	}

	MstrView line = {0}, rest = mk;
	while(!MEOF(line = MstrSplitView(rest, '\n', &rest))) {
		MPrintFmt(MstrViewFmt(line));
    	int level = 0;
    	MstrView garb = line;
    	MstrView parsing = EMPTYVIEW(MstrView);
		switch (line.raw[0]) {
		    case '#':
		    	while(1) {
		    		parsing = MstrTrim(MstrSplitView(garb, '#', &garb));
		    		if (!MEOF(garb)) level++;
		    		else break;
		    	}
		    	
				MStrFmt(
					&html, 
					"<h"$(level)">"MstrViewFmt(parsing)"</h"$(level)">"
				);
		        break;
	        case '-':
		    	while(1) {
		    		parsing = MstrTrim(MstrSplitView(garb, '-', &garb));
		    		if (!MEOF(garb)) level++;
		    		else break;
		    	}
		    	
		    	MPrintFmt($(level));
  				if (level == 3) MStrFmt(&html, "<hr>"); // create a recursinve function to create nested lists
  				if (level < 2) {
  					MStrFmt(&html, "<ul>");
  					MStrFmt(&html, "<li>"MstrViewFmt(parsing)"</li>");
  					while(1) {
			    		line = MstrSplitView(rest, '\n', &rest);
			    		if (MstrTrim(line).raw[0] != '-') break;
			    		MStrFmt(&html, "<li>"MstrViewFmt(MstrTrim(MstrViewFrom(line, 1)))"</li>");
			    	}
  					MStrFmt(&html, "</ul>");
  				}
  		        break;

  			case '!':
  				parsing = MstrTrim(MstrSplitView(garb, '[', &garb));
  				parsing = MstrTrim(MstrSplitView(garb, ']', &garb));
  				MstrView image_alt = parsing;
  				parsing = MstrTrim(MstrSplitView(garb, '(', &garb));
				parsing = MstrTrim(MstrSplitView(garb, ')', &garb));
				MstrView image_url = parsing;

  				MStrFmt(&html, 
  					"<img alt='"MstrViewFmt(image_alt)"' src='"MstrViewFmt(image_url)"'/>");
  		        break;

  		    case '>':
  		    	parsing = MstrTrim(MstrSplitView(garb, '>', &garb));
		    	MStrFmt(&html, "<blockquote><p>"MstrViewFmt(garb)"</p></blockquote>");
   		        break;
   		    default:
   		    	if(line.length > 0)
   		    		MStrFmt(&html, "<p>"MstrViewFmt(garb)"</p>");
		}
	};

	
	MstrView content = MstrViewFrom(MVec(html), 0, MVecLen(html));
	MstrView filen = MStrFmt(&pool, MstrViewFmt(fileName)".html");
	MRetEither(out, err, mfile_create(filen, content));
	if (err) {
		MPrintFmt("mfile_create: %s", strerror(err));
	}
	
	return 0;
}							
