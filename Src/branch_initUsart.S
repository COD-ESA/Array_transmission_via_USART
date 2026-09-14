/*branch_initUsart.S */
/*функция получает адрес функции инициализации усарт и осуществляет 
переход в неё.
*/

.syntax unified         @унифицированный синтаксис
.cpu cortex-m4          @
.thumb


.global jump_initUsart   @сделать символ глобальным для видимости из С
.type jump_initUsart, %function
.thumb_func

jump_initUsart:
    mov r2, r0
    mov r0, r1
    bx r2
