#include <stdio.h>			
#include <stdbool.h>
#include "vendor/mlib/mfile.h"
#include "vendor/mlib/mvector.h"
#include "vendor/mlib/mprint.h"

static char tokens[] = { '#', '*', '-', '.', '|', '>'};
int main(int argc, char** argv) {
	MVecAlloc(pool, char, 500);
	MVecAlloc(html, char, 500);
	if (argc < 2 || argv[1] == NULL) {
		fprintf(stderr, "provdie a markdown\n");
		return 1;
	}
	
	MstrView fileName = MStrFmt(&pool, $(argv[1]));
	MRetEither(mk, mkerr, mfile_read(MVecParamRefPtr(&pool), fileName));
	if (mkerr) {
		MPrintFmt($(mkerr));
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
		MPrintFmt("mfile_create: %s", err);
	}
	
	free(MVec(pool));
	free(MVec(html));
	return 0;
// 	Mstr* token = NULL;
// 	bool inBody = false;
// 	for(;;) {
// 		catch(mstr_chop_by, &token, file_buffer, '\n') {
// 			fprintf(stderr, "Failed to chop md: %s\n", unwrap_fail(token));
// 			return 1;
// 		}
// 		if (token == NULL) break;
// 		mstr_trim(token);
// 		if (token->length == 0) continue;
// 		switch (token->raw[0]) {
// 		    case '#':
// 		        Mstr* content = NULL;
// 				mstr_chop_by_tryfail(&content, token, '#');
// 				MStringPoolPush(&html, MPRINT_FMT_OUT("<h1>"MFMT(token)"</h1>"));
// 				free(content);
// 		        break;
// 		    case '-':
// 		    	Mstr* li = NULL;
// 				mstr_chop_by_tryfail(&li, token, '-');
// 				int count = 0;
// 				while (token->raw[count] == '-') count++;
// 				if (count == 3)
// 					MStringPoolPush(&html, MPRINT_FMT_OUT("<hr>"));
// 				if (count < 2) {
// 					if (inBody) {
// 						MStringPoolPush(&html, MPRINT_FMT_OUT("<li>"MFMT(token)"</li>"));
// 						continue;
// 					} else {
// 						inBody = true;
// 						MStringPoolPush(&html, MPRINT_FMT_OUT("<ul>"));
// 						MStringPoolPush(&html, MPRINT_FMT_OUT("<li>"MFMT(token)"</li>"));
// 						continue;
// 					}	
// 				}
// 				free(li);
// 		        break;
// 		    case '!':
// 		    	Mstr* gbg = NULL;
// 		    	Mstr* image_alt = NULL;
// 		    	Mstr* image_url = NULL;
//    				mstr_chop_by_tryfail(&gbg, token, '[');
//    				mstr_chop_by_tryfail(&image_alt, token, ']');
//    				mstr_chop_by_tryfail(&gbg, token, '(');
//  				mstr_chop_by_tryfail(&image_url, token, ')');
//    				MStringPoolPush(&html, MPRINT_FMT_OUT("<img alt='"MFMT(image_alt)"' src='"MFMT(image_url)"'/>"));
//    				free(gbg);
//    				free(image_alt);
//    				free(image_url);
//    		        break;
//    		    case '*':
// 		    	Mstr* gbgb = NULL;
// 		    	Mstr* bold_content = NULL;
//    		    	
// 	    		mstr_chop_by_tryfail(&gbgb, token, '*');
// 	    		mstr_chop_by_tryfail(&gbgb, token, '*');
// 				catch(mstr_chop_by, &bold_content, token, '*') break;
// 				catch(mstr_chop_by, &gbgb, token, '*') break;
// 				catch(mstr_chop_by, &gbgb, token, '*') break;
// 				MStringPoolPush(&html, MPRINT_FMT_OUT("<b>"MFMT(bold_content)"</b>"));
// 				free(gbgb);
// 				free(bold_content);
//   		        break;
//   		    case '>':
//   		    	Mstr* blockquote = NULL;
//   		    	mstr_chop_by_tryfail(&blockquote, token, '>');
// 		    	MStringPoolPush(&html, MPRINT_FMT_OUT("<blockquote><p>"MFMT(token)"</p></blockquote>"));
// 		    	free(blockquote);
//    		        break;
// 		    default:
// 		        MStringPoolPush(&html, MPRINT_FMT_OUT("<p>"MFMT(token)"</p>"));
// 		}
// 		
// 	    if (inBody) {
// 	    	char * temp = MVecGet(html, (MVecLen(html) - 1));
//     		MVecSet(html, (MVecLen(html) - 1), MPRINT_FMT_OUT("</ul>"));
//     		MStringPoolPush(&html, temp);
//     	 	inBody = false;
// 	    };
// 	    free(token);
// 	}
// 
// 	char* out = NULL;
// 	char* path = MPRINT_FMT_OUT("./"$(fileName)".html");
// 	char* htmlContent = NULL;
// 
// 	MVecForeach(item, html) {
// 		MPRINT_FMT($(item));
// 		htmlContent = MPRINT_FMT_OUT($(htmlContent ? htmlContent : "")$(item));
// 		free(item);
// 		free(htmlContent);
// 	}
// 	
// 	catch(mfile_create, &out, path, htmlContent) {
// 		fprintf(stderr, "Error wrinting html: %s\n", unwrap_fail(out));
// 		free(out);
// 		return 1;
// 	};
// 
// 	free(path);
// 	free(MVec(html));
// 	free(file_buffer);
// 	return 0;				
}							
