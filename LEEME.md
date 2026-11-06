# Vocal Epic Pro 2.0 - Plugin de voces (VST3 para Windows)

Autor: Louis Butrón
© 2026 Louis Butrón. Todos los derechos reservados.

## Cómo obtener el plugin terminado (VS Code + GitHub)

No necesitas instalar Visual Studio: GitHub compila el plugin por ti.

1. Instala **Git** (https://git-scm.com) y **Visual Studio Code**.
2. En GitHub crea un repositorio nuevo y vacío llamado `VocalEpicPro`.
3. Abre esta carpeta en VS Code (Archivo > Abrir carpeta).
4. Abre la terminal de VS Code (Terminal > Nueva terminal) y ejecuta, uno por uno:

       git init
       git add .
       git commit -m "Vocal Epic Pro 2.0"
       git branch -M main
       git remote add origin https://github.com/TU_USUARIO/VocalEpicPro.git
       git push -u origin main

5. En GitHub abre la pestaña **Actions**. Verás "Compilar Vocal Epic Pro (Windows)" trabajando (tarda unos 10 minutos la primera vez).
6. Cuando salga en verde, entra en esa ejecución y baja a **Artifacts**. Descarga **VocalEpicPro-2.0-Windows-VST3**.

## Instalarlo
1. Descomprime lo descargado.
2. Dentro de la carpeta `VST3` está `Vocal Epic Pro.vst3`. Cópiala a: `C:\Program Files\Common Files\VST3`
3. FL Studio: Options > Manage plugins > Find installed plugins.
   Ableton: Preferences > Plug-ins > activa VST3 y pulsa Rescan.

## Cadena de audio (en orden)
Entrada > filtro de graves > Calidez / Presencia / Aire > Anti-sibilancia > Compresión > Saturación > Eco > Reverb > Mezcla > Salida

## Presets incluidos (12)
Reggaetón: Voz Principal, Segunda Voz, Coros
Trap: Voz Principal, Segunda Voz (Ad-libs)
Afrobeats: Voz Principal, Coros
Deep House: Voz Principal, Coros Etéreos
General: Segundas Voces, Coros, Voz Limpia Natural

## Licencia de JUCE
JUCE es gratis para uso personal y de código abierto. Si vas a vender el plugin, necesitas una licencia comercial de JUCE.
