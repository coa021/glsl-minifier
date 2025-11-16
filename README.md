# GLSL Shader Minifier
Small project that minifies glsl shader code.

## Building the Project
```
git clone https://github.com/coa021/glsl-minifier.git
cd glsl-minifier
mkdir build
cd build
cmake -S .. -B .
cmake --build .
```

## Usage

Usage:  
Minify:  
```glsl_minifier minify <input.glsl> [output.glsl] [options]```  
Render:  
```glsl_minifier render <shader.glsl>```  
Help:  
```glsl_minifier --help```  
Options:  
```--verify``` Run correctness verification (compile and compare)  
```--dead-code``` Show dead code analysis  
Examples:  
```glsl_minifier minify shader.glsl out.glsl```  
```glsl_minifier minify shader.glsl out.glsl --verify --dead-code```  
```glsl_minifier render shader.glsl```  