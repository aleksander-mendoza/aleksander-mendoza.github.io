define("ace/mode/mealy_highlight_rules",["require","exports","module","ace/lib/oop","ace/mode/text_highlight_rules"], function(require, exports, module) {
"use strict";
var oop = require("../lib/oop");
var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;
var MealyHighlightRules = function() {
this.$rules = {
"start" : [
   {
      "token" : ["entity.name.type", "none", "meta.separator", "none", "punctuation"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(=)(\\s*)(\\[)",
      "push" : "alphabet"
   },
   {
      "token" : "comment",
      "regex" : "(//[^\\n]*)"
   },
   {
      "token" : "meta.separator",
      "regex" : "(;)"
   },
   {
      "token" : ["variable", "none", "punctuation", "none", "punctuation", "none", "meta.separator"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(\\()(\\s*)(\\))(\\s*)(=)",
      "push" : "function"
   },
   {
      "token" : ["entity.name.type", "none", "meta.separator"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(:)",
      "push" : "type"
   },
   {
      "token" : ["keyword", "none", "entity.name.type", "none", "punctuation"],
      "regex" : "(struct)(\\s+)([a-zA-Z0-9_]+)(\\s*)(\\{)",
      "push" : "struct"
   },
   {
      defaultToken : "text",
   }
], 
"alphabet" : [
   {
      "token" : ["punctuation", "none", "punctuation"],
      "regex" : "(\\])(\\s*)(;)",
      "next" : "pop"
   },
   {
      "token" : "keyword",
      "regex" : "(#)"
   },
   {
      "token" : "constant.character.escape",
      "regex" : "(\\\\.)"
   },
   {
      "token" : "text",
      "regex" : "(.)"
   },
   {
      defaultToken : "text",
   }
], 
"function" : [
   {
      "token" : "meta.separator",
      "regex" : "(;)",
      "next" : "pop"
   },
   {
      "token" : "constant.numeric",
      "regex" : "(\\b[0-9]+\\b)"
   },
   {
      "token" : "keyword",
      "regex" : "(->|\\*)"
   },
   {
      "token" : "keyword",
      "regex" : "(:|\\*|\\+|\\|)"
   },
   {
      "token" : "punctuation",
      "regex" : "(\\(|\\))"
   },
   {
      "token" : "string.regexp",
      "regex" : "(\\[([^\\\\\\[\\]\\-]|\\\\.)\\-([^\\\\\\[\\]\\-]|\\\\.)\\])"
   },
   {
      "token" : "variable",
      "regex" : "([a-zA-Z0-9_]+)"
   },
   {
      "token" : "punctuation",
      "regex" : "(\")",
      "push" : "string"
   },
   {
      defaultToken : "text",
   }
], 
"string" : [
   {
      "token" : "punctuation",
      "regex" : "(\")",
      "next" : "pop"
   },
   {
      "token" : "keyword",
      "regex" : "(#)"
   },
   {
      "token" : "constant.character.escape",
      "regex" : "(\\\\.)"
   },
   {
      "token" : "string",
      "regex" : "(.)"
   },
   {
      defaultToken : "text",
   }
], 
"struct" : [
   {
      "token" : "punctuation",
      "regex" : "(})",
      "next" : "pop"
   },
   {
      "token" : ["variable", "none", "meta.separator", "none", "entity.name.type", "none", "keyword"],
      "regex" : "([a-zA-Z0-9_]+)(\\s*)(:)(\\s*)([a-zA-Z0-9_]+)(\\s*)(\\*?)"
   },
   {
      "token" : "meta.separator",
      "regex" : "(,)"
   },
   {
      defaultToken : "text",
   }
], 
"type" : [
   {
      "token" : "meta.separator",
      "regex" : "(;)",
      "next" : "pop"
   },
   {
      "token" : "entity.name.type",
      "regex" : "([a-zA-Z0-9_]+)"
   },
   {
      "token" : "keyword",
      "regex" : "(->|\\*)"
   },
   {
      defaultToken : "text",
   }
]
};
this.normalizeRules();
};
oop.inherits(MealyHighlightRules, TextHighlightRules);
exports.MealyHighlightRules = MealyHighlightRules;
});

define("ace/mode/folding/cstyle",["require","exports","module","ace/lib/oop","ace/range","ace/mode/folding/fold_mode"], function(require, exports, module) {
"use strict";

var oop = require("../../lib/oop");
var Range = require("../../range").Range;
var BaseFoldMode = require("./fold_mode").FoldMode;

var FoldMode = exports.FoldMode = function(commentRegex) {
    if (commentRegex) {
        this.foldingStartMarker = new RegExp(
            this.foldingStartMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.start)
        );
        this.foldingStopMarker = new RegExp(
            this.foldingStopMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.end)
        );
    }
};
oop.inherits(FoldMode, BaseFoldMode);

(function() {
    
    this.foldingStartMarker = /([\{\[\(])[^\}\]\)]*$|^\s*(\/\*)/;
    this.foldingStopMarker = /^[^\[\{\(]*([\}\]\)])|^[\s\*]*(\*\/)/;
    this.singleLineBlockCommentRe= /^\s*(\/\*).*\*\/\s*$/;
    this.tripleStarBlockCommentRe = /^\s*(\/\*\*\*).*\*\/\s*$/;
    this.startRegionRe = /^\s*(\/\*|\/\/)#?region\b/;
    this._getFoldWidgetBase = this.getFoldWidget;
    this.getFoldWidget = function(session, foldStyle, row) {
        var line = session.getLine(row);
    
        if (this.singleLineBlockCommentRe.test(line)) {
            if (!this.startRegionRe.test(line) && !this.tripleStarBlockCommentRe.test(line))
                return "";
        }
    
        var fw = this._getFoldWidgetBase(session, foldStyle, row);
    
        if (!fw && this.startRegionRe.test(line))
            return "start"; // lineCommentRegionStart
    
        return fw;
    };

    this.getFoldWidgetRange = function(session, foldStyle, row, forceMultiline) {
        var line = session.getLine(row);
        
        if (this.startRegionRe.test(line))
            return this.getCommentRegionBlock(session, line, row);
        
        var match = line.match(this.foldingStartMarker);
        if (match) {
            var i = match.index;

            if (match[1])
                return this.openingBracketBlock(session, match[1], row, i);
                
            var range = session.getCommentFoldRange(row, i + match[0].length, 1);
            
            if (range && !range.isMultiLine()) {
                if (forceMultiline) {
                    range = this.getSectionRange(session, row);
                } else if (foldStyle != "all")
                    range = null;
            }
            
            return range;
        }

        if (foldStyle === "markbegin")
            return;

        var match = line.match(this.foldingStopMarker);
        if (match) {
            var i = match.index + match[0].length;

            if (match[1])
                return this.closingBracketBlock(session, match[1], row, i);

            return session.getCommentFoldRange(row, i, -1);
        }
    };
    
    this.getSectionRange = function(session, row) {
        var line = session.getLine(row);
        var startIndent = line.search(/\S/);
        var startRow = row;
        var startColumn = line.length;
        row = row + 1;
        var endRow = row;
        var maxRow = session.getLength();
        while (++row < maxRow) {
            line = session.getLine(row);
            var indent = line.search(/\S/);
            if (indent === -1)
                continue;
            if  (startIndent > indent)
                break;
            var subRange = this.getFoldWidgetRange(session, "all", row);
            
            if (subRange) {
                if (subRange.start.row <= startRow) {
                    break;
                } else if (subRange.isMultiLine()) {
                    row = subRange.end.row;
                } else if (startIndent == indent) {
                    break;
                }
            }
            endRow = row;
        }
        
        return new Range(startRow, startColumn, endRow, session.getLine(endRow).length);
    };
    this.getCommentRegionBlock = function(session, line, row) {
        var startColumn = line.search(/\s*$/);
        var maxRow = session.getLength();
        var startRow = row;
        
        var re = /^\s*(?:\/\*|\/\/|--)#?(end)?region\b/;
        var depth = 1;
        while (++row < maxRow) {
            line = session.getLine(row);
            var m = re.exec(line);
            if (!m) continue;
            if (m[1]) depth--;
            else depth++;

            if (!depth) break;
        }

        var endRow = row;
        if (endRow > startRow) {
            return new Range(startRow, startColumn, endRow, line.length);
        }
    };

}).call(FoldMode.prototype);

});

define("ace/mode/mealy",["require","exports","module","ace/lib/oop","ace/mode/text","ace/mode/mealy_highlight_rules","ace/mode/folding/cstyle"], function(require, exports, module) {
"use strict";

var oop = require("../lib/oop");
var TextMode = require("./text").Mode;
var MealyHighlightRules = require("./mealy_highlight_rules").MealyHighlightRules;
var FoldMode = require("./folding/cstyle").FoldMode;

var Mode = function() {
    this.HighlightRules = MealyHighlightRules;
    this.foldingRules = new FoldMode();
    this.$behaviour = this.$defaultBehaviour;
};
oop.inherits(Mode, TextMode);

(function() {
    this.lineCommentStart = "//";
    this.$id = "ace/mode/mealy";
    this.snippetFileId = "ace/snippets/mealy";
}).call(Mode.prototype);

exports.Mode = Mode;
});
                (function() {
                    window.require(["ace/mode/mealy"], function(m) {
                        if (typeof module == "object" && typeof exports == "object" && module) {
                            module.exports = m;
                        }
                    });
                })();
            