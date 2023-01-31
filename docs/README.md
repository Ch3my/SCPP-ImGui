## Implementacion de SCPP en ImGUI

Para saber como instalar ImGUi desde cero revisar el video https://www.youtube.com/watch?v=VRwhNKoxUtk.
Como nota, la clave esta en copiar los cpp y los .h a la carpeta includes y agregarlas al proyecto
para que el IDE sepa que los tiene que buscar.

Por compatiblidad con versiones, por defecto visual studio esta usando C++14, asi que nos ajustamos
a esa especificacion, en el futuro se podria cambiar la version a algo mas nuevo. Verificando por
supuesto que ImGUI sea compatible

El proyecto inicio usando DirectX12 como el backend, luego de enterarse backend que eran compatibles
para Windows y para Linux se cambio a usar Vulkan. Se espera en algun momento saber como se podria
compilar para Linux

Como configurar Vulkan aqui: https://vulkan-tutorial.com/Development_environment

Al cambiarse a Vulkan fue necesario eliminar las referencias a impl_directx porque parece que entraban en conflicto. 
El proyecto puede volver a Direct12 cambiando el App.cpp al que tiene Direct12 y agregando los archivos Impl de Directx12
de nuevo al proyecto. NOTA, los archivos estan el la carpeta pero se quitaron del proyecto solamente

NOTA

ImGUI no tiene un datePicker asi que se tuvo que incluir un addon (desde https://github.com/Flix01/imgui) y este usa funciones deprecadas
se incluyo _CRT_SECURE_NO_WARNINGS en Project -> Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions" add _CRT_SECURE_NO_WARNINGS. Deberia modificar el archivo para eliminar las funciones deprecadas, no son muchas.