#include <stdio.h>		
#include <stdbool.h>
#include "vendor/mlib/mvec.h"
#include "vendor/mlib/mfile.h"
#include "vendor/mlib/mprint.h"

int main(int argc, char** argv) {
	__free(strfree) char* MVecAlloc(pool, 500);
	__free(strfree) char* MVecAlloc(html, 500);

	if (argc < 2 || argv[1] == NULL) {
		MPrintFmt("provdie a markdown");
		return 1;
	}
	
	MstrView mk = MfileReadCstr(&MVecRef(pool), argv[1]);
	if (IsEmptyView(mk)) {
		MPrintFmt("File read fail: "$(errno));
		return 1;
	}

	MstrView line = {0}, rest = mk;
	while(!IsEmptyView(line = MstrSplitView(rest, '\n', &rest))) {
		MPrintFmt(MstrViewFmt(line));
    	int level = 0;
    	MstrView garb = line;
    	MstrView parsing = EMPTYVIEW;
		switch (line.raw[0]) {
		    case '#':
		    	while(1) {
		    		parsing = MstrTrim(MstrSplitView(garb, '#', &garb));
		    		if (!IsEmptyView(garb)) level++;
		    		else break;
		    	}
		    	
				MStrFmt(&MVecRef(html), "<h"$(level)">"MstrViewFmt(parsing)"</h"$(level)">");
		        break;
	        case '-':
		    	while(1) {
		    		parsing = MstrTrim(MstrSplitView(garb, '-', &garb));
		    		if (!IsEmptyView(garb)) level++;
		    		else break;
		    	}
		    	
		    	MPrintFmt($(level));
  				if (level == 3) MStrFmt(&MVecRef(html), "<hr>"); // create a recursinve function to create nested lists
  				if (level < 2) {
  					MStrFmt(&MVecRef(html), "<ul>");
  					MStrFmt(&MVecRef(html), "<li>"MstrViewFmt(parsing)"</li>");
  					while(1) {
			    		line = MstrSplitView(rest, '\n', &rest);
			    		if (MstrTrim(line).raw[0] != '-') break;
			    		MStrFmt(&MVecRef(html), "<li>"MstrViewFmt(MstrTrim(MstrViewFrom(line, 1)))"</li>");
			    	}
  					MStrFmt(&MVecRef(html), "</ul>");
  				}
  		        break;

  			case '!':
  				parsing = MstrTrim(MstrSplitView(garb, '[', &garb));
  				parsing = MstrTrim(MstrSplitView(garb, ']', &garb));
  				MstrView image_alt = parsing;
  				parsing = MstrTrim(MstrSplitView(garb, '(', &garb));
				parsing = MstrTrim(MstrSplitView(garb, ')', &garb));
				MstrView image_url = parsing;

  				MStrFmt(&MVecRef(html), 
  					"<img alt='"MstrViewFmt(image_alt)"' src='"MstrViewFmt(image_url)"'/>");
  		        break;

  		    case '>':
  		    	parsing = MstrTrim(MstrSplitView(garb, '>', &garb));
		    	MStrFmt(&MVecRef(html), "<blockquote><p>"MstrViewFmt(garb)"</p></blockquote>");
   		        break;
   		    default:
   		    	if(line.length > 0)
   		    		MStrFmt(&MVecRef(html), "<p>"MstrViewFmt(garb)"</p>");
		}
	};

	
	MstrView content = MstrViewFrom(MVec(html), 0, MVecLen(html));
	MstrView filen = MStrFmt(&MVecRef(pool), $(argv[1])".html");
	MstrView out = MFileCreate(filen, content);
	if (IsEmptyView(out)) {
		MPrintFmt("MFileCreate: %s", strerror(errno));
	}
	
	return 0;
}							
