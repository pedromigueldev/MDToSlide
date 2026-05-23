#include <stdio.h>
#include <stdbool.h>
#include "vendor/mlib/marr.h"
#include "vendor/mlib/mfile.h"
#include "vendor/mlib/mprint.h"

int main(int argc, char** argv) {
	MByteArray* file __free(MByteArrayFree) = MByteArrayMalloc(100);
	MByteArray* html __free(MByteArrayFree) = MByteArrayMalloc(100);
	if (argc < 2 || argv[1] == NULL) {
		MPrintFmt("provdie a markdown");
		return 1;
	}

	MstrView mk = MfileReadCstr(&file, argv[1]);
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

				MStrFmt(&html, "<h"$(level)">"MstrViewFmt(parsing)"</h"$(level)">");
		        break;
	        case '-':
		    	while(1) {
		    		parsing = MstrTrim(MstrSplitView(garb, '-', &garb));
		    		if (!IsEmptyView(garb)) level++;
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


	MstrView content = MstrViewFrom(html->raw, 0, html->len);
	MstrView filen = MStrFmt(&file, $(argv[1])".html");
	MstrView out = MfileWrite(filen, content);
	if (IsEmptyView(out)) {
		MPrintFmt("MFileCreate: %s", strerror(errno));
	}

	return 0;
}
