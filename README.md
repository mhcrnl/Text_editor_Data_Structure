						Data Structure for Text Editor

						
For a text editor, a text need to be represented in a such a way that insert,delete can be done without copying all characters beyound the insertion point. The classical technique for this is to represent the text as a sequence of lines, where the lines are organized in a strucutre that allows access to the ith line, or to insert a new line immediately before the ith line.

We need a strucutre with a number, the index of the line, as key, and it returns a line of text, represented by a char* object. We can then change this line, and save it in the strucutre again, and possibly we want to insert a new line immediately before the ith line, by which the new line becomes the ith line and every following line is renumbered. The lines are numbered from 1 to n with an artificial empty line n+1 as end marker.

An implementation of lines using linkedin list or any other strucutre that explicitely renumbers the lines will be too slow. Thus this implementation is based on a balanced search tree , an the key mechanism is designed in such a way that we can easily increase the keys of all leaves above a certain key, without visiting more than O(log n) nodes.