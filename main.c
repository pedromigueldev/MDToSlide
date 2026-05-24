#include <stdio.h>
#include <stdbool.h>
#include "vendor/mlib/marr.h"
#include "vendor/mlib/mfile.h"
#include "vendor/mlib/mprint.h"

int char_eq(char a, char b) { return a == b; }

int main(int argc, char** argv) {
	MByteArray* file __free(MByteArrayFree) = MByteArrayMalloc(100);
	MByteArray* html __free(MByteArrayFree) = MByteArrayMalloc(100);
	if (argc < 2 || argv[1] == NULL) {
		MPrintFmt("provdie a markdown");
		return 1;
	}

	MstrView mk = MfileReadCstr(&file, argv[1]);
	if (MstrViewIsEmpty(mk)) {
		MPrintFmt("File read fail: "$(errno));
		return 1;
	}

	MstrView line = {0}, rest = mk;
	while(!MstrViewIsEmpty(line = MstrViewSplit(rest, '\n', char_eq, &rest))) {
		MPrintFmt(MstrViewFmt(line));
    	int level = 0;
    	MstrView garb = line;
    	MstrView parsing = MstrViewEmpty();
		switch (*MstrViewRaw(line)) {
		    case '#':
		    	while(1) {
		    		parsing = MstrViewTrim(MstrViewSplit(garb, '#', char_eq, &garb), ' ', char_eq);
		    		if (!MstrViewIsEmpty(garb)) level++;
		    		else break;
		    	}

				MStrFmt(&html, "<h"$(level)">"MstrViewFmt(parsing)"</h"$(level)">");
		        break;
	        case '-':
		    	while(1) {
		    		parsing = MstrViewTrim(MstrViewSplit(garb, '-', char_eq, &garb), ' ', char_eq);
		    		if (!MstrViewIsEmpty(garb)) level++;
		    		else break;
		    	}

  				if (level == 3) MStrFmt(&html, "<hr>"); // create a recursinve function to create nested lists
  				if (level < 2) {
  					MStrFmt(&html, "<ul>");
  					MStrFmt(&html, "<li>"MstrViewFmt(parsing)"</li>");
  					while(1) {
			    		line = MstrViewSplit(rest, '\n', char_eq, &rest);
			    		if (*MstrViewRaw(MstrViewTrim(line, ' ', char_eq)) != '-') break;
			    		line.start++;
			    		MStrFmt(&html, "<li>"MstrViewFmt(MstrViewTrim(line, ' ', char_eq))"</li>");
			    	}
  					MStrFmt(&html, "</ul>");
  				}
  		        break;
			case '`':
				if (!MstrViewIsEmpty(MstrViewFindSpan(line, "```", 3, char_eq))) {
					garb = line;
					garb.start += 3;
					garb.length -= 3;
					MstrView lang = MstrViewTrim(garb, ' ', char_eq);
					
					if (!MstrViewIsEmpty(lang))
						MStrFmt(&html, "<pre><code class=\"language-"MstrViewFmt(lang)"\">");
					else
						MStrFmt(&html, "<pre><code>");

					while (!MstrViewIsEmpty(rest)) {
						MstrView code_line = MstrViewSplit(rest, '\n', char_eq, &rest);
						if (!MstrViewIsEmpty(MstrViewFindSpan(code_line, "```", 3, char_eq)))
							break;
						
						if (!MstrViewIsEmpty(code_line))
							MStrFmt(&html, MstrViewFmt(code_line));
						MStrFmt(&html, "\n");
					}
					
					MStrFmt(&html, "</code></pre>");
				} else {
					garb = line;
					garb.start++;
					garb.length--;
					MstrView code = MstrViewSplit(garb, '`', char_eq, &garb);
					MStrFmt(&html, "<code>"MstrViewFmt(code)"</code>");
				}
			break;
  			case '!':
  				parsing = MstrViewSplit(garb, '[', char_eq, &garb);
  				parsing = MstrViewSplit(garb, ']', char_eq, &garb);
  				MstrView image_alt = parsing;
  				parsing = MstrViewSplit(garb, '(', char_eq, &garb);
				parsing = MstrViewSplit(garb, ')', char_eq, &garb);
				MstrView image_url = parsing;

  				MStrFmt(&html, "<img alt='"MstrViewFmt(image_alt)"' src='"MstrViewFmt(image_url)"'/>");
  		        break;
			case '[':
  				parsing = MstrViewTrim(MstrViewSplit(garb, '[', char_eq, &garb), ' ', char_eq);
  				parsing = MstrViewTrim(MstrViewSplit(garb, ']', char_eq, &garb), ' ', char_eq);
  				MstrView label = parsing;
  				parsing = MstrViewTrim(MstrViewSplit(garb, '(', char_eq, &garb), ' ', char_eq);
				parsing = MstrViewTrim(MstrViewSplit(garb, ')', char_eq, &garb), ' ', char_eq);
				MstrView link = parsing;

  				MStrFmt(&html,
  					"<a href='"MstrViewFmt(link)"'>"MstrViewFmt(label)"</a>");
  		        break;
  		    case '>':
  		    	parsing = MstrViewTrim(MstrViewSplit(garb, '>', char_eq, &garb), ' ', char_eq);
		    	MStrFmt(&html, "<blockquote><p>"MstrViewFmt(garb)"</p></blockquote>");
   		        break;
   		    default:
   		    	if(line.length > 0)
   		    		MStrFmt(&html, "<p>"MstrViewFmt(garb)"</p>");
		}
	};


	MstrView content = (MstrView){ .start = 0, .length = html->len, .raw = &html };
	MstrView filen = MStrFmt(&file, $(argv[1])".html");
	MstrView out = MfileWrite(filen, content);
	if (MstrViewIsEmpty(out)) {
		MPrintFmt("MFileCreate: %s", strerror(errno));
	}

	return 0;
}
