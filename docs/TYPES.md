This document the basic types recognized/used by Vanity.

For requests expecting an object, the type is known and implicitly assumed, so the the type annotation is not included.  
In Responses, the type is always included.

An `<object>` is defined as a sequence of characters, with the following structure:  
`[<type_annotation>]<value>`

The `<type_annotation>` has the following structure:  
`:<type_name>`  
where `<type_name>` is the name of the type, and is one of the following:
- STR for a `<string>`
- INT for an `<int>`
- FLOAT for a `<float>`
- BOOL for a `<bool>`
- NULL for a `<null>`
- ARR for an `<arr>`
- LIST for a `<list>`
- SET for a `<set>`
- HASH for a `<hash>`
- PIPE for a `<pipe>`

Note: Currently, no type annotations are used in requests, but they are used in responses.

We define a `<length>` thus:
 - `(len)`  
    where  
    - len is the length of the object encoded as an integer, and surrounded by the parentheses.
    
    Note: The parentheses are part of the syntax, and are not optional.
    Whitespace is not allowed between the parentheses and the length, or between the `<length>` and the preceding object.

The `<value>` is the value of the object, and is defined for each type as follows:
- `<string>` => `<length><string_body>`  
    where  
    - `<length>` is a length as defined above,  
    - `<string_body>` is the string itself. Exactly len characters are read after the opening closing parentheses and no escape sequences are recognized.

- `<int>` => `<int_as_string>`  
    where  
    - `<int_as_string>` is the integer itself, encoded as a string of digits, and preceded by an optional minus sign.  
    
    It would be necessary to add a whitespace after the INT in the case the following object, if any, is also numeric. This is not necessary if the following object is not numeric.

- `<float>` => `<float_as_string>`  
    where  
    - `<float_as_string>` is the floating point number itself
    
    It would be necessary to add a whitespace after the FLOAT in the case the following object, if any, is also numeric. This is not necessary if the following object is not numeric.

- `<bool>` => `true or false`  
    Note: Currently, it is an error to specify a BOOL in a request, but they are used in responses.

- `<null>`  
    The NULL type has no value, and is represented by the empty string (the presence of the type annotation is enough to identify it).  
    Note: Currently, it is an error to specify a NULL in a request, but they are used in responses.

- `<arr>` => `<length>[<object>...] or <length>[<string>...]`  
    where  
    - `<object>` is an object of any type, including annotations, and the square brackets are part of the syntax.
    - `<string>` is a string as defined above, and the square brackets are part of the syntax.

    Note: In requests, a sequence of unannotated strings are expected. In a response, it is used to represent a sequence of objects of any type, including annotations.

- `<list>` => `<length>[<string>...]`  
    where  
    - `<length>` is a length as defined above,
    - `<string>` is a string as defined above, and the square brackets are part of the syntax.  
    
    Note: LIST is only allowed to contain strings.

- `<set>` => `<length>{<string>...}`  
    where  
    - `<length>` is a length as defined above,
    - `<string>` is a string as defined above, and the curly braces are part of the syntax.

    Note: SET is only allowed to contain strings.

- `<hash>` => `<length>{<string><string>...}`  
    where  
    - `<length>` is a length as defined above,
    - `<string>` is a string as defined above, and the curly braces are part of the syntax.

    Note: HASH is only allowed to contain strings, and the strings are always in pairs, with the first string being the key, and the second string being the value.

- `<pipe>` => `<length><pipe_body>`  
    where  
    - `<length>` is a length as defined above,
    - `<pipe_body>` is the pipe itself. Exactly len characters are read after the opening closing parentheses and no escape sequences are recognized.

- `<pipe_body>` => `<request>... or <response>...`  
    where  
    - `<request>` is a request as defined in the SYNTAX documentation,
    - `<response>` is a response as defined in the SYNTAX documentation,

    No extra characters are expected to demarcate the sequence of requests/responses.