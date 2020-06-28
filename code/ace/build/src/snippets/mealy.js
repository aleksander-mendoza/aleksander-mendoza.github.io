define("ace/snippets/mealy",["require","exports","module"], function(require, exports, module) {
"use strict";

exports.snippetText = "snippet >\n\
description function\n\
scope mealy\n\
	-> ${1}() = ${2};\n\
\n\
snippet >\n\
description if\n\
scope mealy\n\
	-> struct ${1} \\{ ${2:**} \\n \\}\n\
";
exports.scope = "mealy";

});
                (function() {
                    window.require(["ace/snippets/mealy"], function(m) {
                        if (typeof module == "object" && typeof exports == "object" && module) {
                            module.exports = m;
                        }
                    });
                })();
            