#include "ui_language.h"

#include <cwchar>
#include <filesystem>
#include <fstream>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

// ---------------------------------------------------------------------------
// English text.
//
// Every entry below pairs the Spanish string used by the code with its English
// translation. To translate the launcher and the settings overlay, replace
// `nullptr` with the English literal, for example:
//
//     {L"Preparar el juego", L"Prepare the game"},
//
// Rules:
//   * Keep the Spanish string on the left exactly as it is; it is the lookup key.
//   * Format strings must keep their tokens. For instance
//     L"Sensibilidad del raton: %d%%" needs the same %d%% in English.
//   * An empty or nullptr English string means "not translated yet"; the UI
//     then keeps showing the Spanish text.
// ---------------------------------------------------------------------------
namespace {

struct UiTranslation {
    const wchar_t* spanish;
    const wchar_t* english;
};

constexpr UiTranslation kTranslations[] = {
    // ---- Settings overlay: sidebar -------------------------------------------------
    {L"Gráficos", L"Graphics"},
    {L"Apariencia", L"Appearance"},
    {L"Sistema", L"System"},
    {L"Controles", L"Controls"},
    {L"Gestor de Guardados", L"Save Manager"},
    {L"Volver al juego", L"Back to the game"},

    // ---- Settings overlay: Graphics -------------------------------------------------
    {L"Pantalla, rendimiento y presentación", L"Screen, performance and presentation"},
    {L"Contador de FPS", L"FPS counter"},
    {L"Rendimiento en pantalla", L"Overlays FPS on screen"},
    {L"Sincroniza con el monitor", L"Synchronize FPS with monitor refresh rate"},
    {L"Resolución interna", L"Internal resolution"},
    {L"Más alta = imagen más nítida y suave (supersampling).", L"Higher = sharper and smoother image (supersampling)."},
    {L"Límite de FPS", L"FPS limit"},
    {L"La simulación conserva el tiempo real.", L"The simulation preserves real-time."},
    {L"Renderizador gráfico", L"Graphics renderer"},
    {L"Se aplica al reiniciar el juego", L"Applies when restarting the game"},
    {L"Relación 16:9", L"16:9 aspect ratio"},
    {L"Proporción panorámica", L"Panning ratio"},
    {L"Maximizar ventana", L"Maximize window"},
    {L"Ajustar a la pantalla", L"Fit to screen"},
    {L"Pantalla completa", L"Full screen"},
    {L"Ocupa todo el monitor, sin bordes", L"Occupies the entire monitor, without borders"},
    {L"Restaurar ajustes predeterminados", L"Restore default settings"},

    // ---- Settings overlay: Appearance ----------------------------------------------
    {L"Tema, bordes y tipografía", L"Theme, borders and font"},
    {L"Pergamino Guilda", L"Guild Scroll"},
    {L"Tribal de Tri", L"Tribe of Tri"},
    {L"Aguas cristalinas", L"Crystal Waters"},
    {L"Brasas y escamas", L"Embers and Scales"},
    {L"Barro y hueso", L"Clay and Bone"},
    {L"Abismo electrico", L"Electric Abyss"},
    {L"Selva y plumas", L"Jungle and Feathers"},
    {L"Aurora cosmica", L"Cosmic Aurora"},
    {L"Voraz y acido", L"Ravenous and Acidic"},
    {L"Brisa clara", L"Clear Breeze"},
    {L"Negro y oro", L"Black and Gold"},
    {L"Bordes", L"Borders"},
    {L"Redondeados o rectos", L"Rounded or straight"},
    {L"Redondeados", L"Rounded"},
    {L"Rectos", L"Straight"},
    {L"Fuente de interfaz", L"Interface font"},
    {L"Tipografía del panel y del contador de FPS.", L"Font of the overlay and the FPS counter."},

    // ---- Settings overlay: System ---------------------------------------------------
    {L"Idioma, guardados y reinicio", L"Language, saves and restart"},
    {L"Idioma del juego", L"Game language"},
    {L"Texto y menús del disco europeo. Requiere reiniciar.", L"Text and menus of the European disc. Requires restart."},
    {L"Reinicia el port para aplicar el cambio de idioma.", L"Restart the port to apply the language change."},
    {L"Idioma activo: ", L"Active language: "},
    {L"Idioma activo: el del sistema (automático).", L"Active language: system (automatic)."},
    {L"Idioma de la interfaz", L"Interface language"},
    {L"Texto del panel y del lanzador.", L"Text of the overlay and launcher."},
    {L"Reiniciar el port", L"Restart the port"},
    {L"Aplica el idioma y otros cambios.", L"Applies the language and other changes."},
    {L"Reiniciar ahora", L"Restart now"},
    {L"Guardados", L"Save files"},
    {L"Abrir la carpeta Saves", L"Open the Saves folder"},
    {L"Carpeta del port", L"Port folder"},
    {L"Donde está MH3.exe", L"Directory of MH3.exe"},
    {L"Consejo: haz una copia de Saves antes de actualizar el port.", L"Tip: make a copy of Saves before updating the port."},

    // ---- Settings overlay: Controls -------------------------------------------------
    {L"Atajos y navegación del panel", L"Shortcuts and overlay navigation"},
    {L"Abrir o cerrar ajustes", L"Open or close settings"},
    {L"Cambiar de pestaña", L"Change tab"},
    {L"Elegir una opción", L"Select an option"},
    {L"Salir del panel", L"Close the overlay"},
    {L"Alterna este panel mientras juegas.", L"Toggle settings overlay."},
    {L"Recorre las secciones del panel.", L"Navigate the sections of the panel."},
    {L"Usa el ratón sobre botones y tarjetas.", L"Use the mouse over buttons and cards."},
    {L"Vuelve al juego.", L"Return to the game."},
    {L"CLIC", L"CLICK"},
    {L"Los cambios se guardan al instante. El idioma y el reinicio están en Sistema.", L"The changes are saved instantly. The language and restart options are in System."},
    {L"Sensibilidad del raton: %d%%", L"Mouse sensitivity: %d%%"}, // keep the %d%% tokens

    // ---- Settings overlay: Save Manager page ----------------------------------------
    {L"Crea una copia de data en UserData/SaveBackups; no modifica la partida activa.", L"Creates a copy of data in UserData/SaveBackups; does not modify the active save."},
    {L"Importar partida", L"Import save"},
    {L"Exportar partida", L"Export save"},
    {L"Selecciona una exportación descomprimida: carpeta data o title\\00010000\\524d4850\\data.", L"Select a decompressed export: data folder or title\\00010000\\524d4850\\data."},
    {L"Selecciona la carpeta donde se guardará la exportación.", L"Select the folder where the export will be saved."},
    {L"Carpeta superior", L"Parent folder"},
    {L"Elegir esta carpeta", L"Choose this folder"},
    {L"Guardar aquí", L"Save here"},
    {L"Cancelar", L"Cancel"},
    {L"Anterior", L"Previous"},
    {L"Siguiente", L"Next"},
    {L"Confirmar importación", L"Confirm import"},
    {L"Carpeta seleccionada:", L"Selected folder:"},
    {L"Al confirmar, la carpeta queda preparada. La partida actual no se reemplaza ahora: al cerrar MH3 se crea primero un backup y después se aplica la importación.", L"When confirmed, the folder is prepared. The current save is not replaced now: when closing MH3, a backup is created first and then the import is applied."},
    {L"Backup y Exportar se ejecutan al cerrar MH3, después de guardar. Importar también se aplica al cerrar.", L"Backup and Export are executed when closing MH3, after saving. Import is also applied when closing."},
    {L"Backup", L"Backup"},
    {L"Programa una copia fechada en SaveBackups para crearla al cerrar MH3, después del guardado.", L"Schedule a dated copy in SaveBackups to be created when closing MH3, after saving."},
    {L"Importar", L"Import"},
    {L"Elige una carpeta exportada con data. Al cerrar, crea un backup y después reemplaza la partida.", L"Choose an exported folder with data. When closing, create a backup and then replace the save."},
    {L"Exportar", L"Export"},
    {L"Programa MH3-RMHP08 con data y LEEME.txt en el destino; se crea al cerrar MH3.", L"Schedule MH3-RMHP08 with data and LEEME.txt in the destination; it is created when closing MH3."},

    // ---- Settings overlay: Save Manager status messages / native dialog -------------
    {L"No se puede leer esta carpeta. Vuelve a la carpeta superior y elige otra.", L"Cannot read this folder. Go back to the parent folder and choose another."},
    {L"Windows no pudo cargar los componentes del selector de carpetas.", L"Windows could not load the components of the folder selector."},
    {L"Windows no expone las funciones necesarias para el selector de carpetas.", L"Windows does not expose the necessary functions for the folder selector."},
    {L"Windows no pudo inicializar el selector de carpetas.", L"Windows could not initialize the folder selector."},
    {L"Windows no pudo crear el selector de carpetas.", L"Windows could not create the folder selector."},
    {L"Selección cancelada. No se modificó la partida.", L"Selection canceled. The save was not modified."},
    {L"No se pudo abrir el selector nativo de carpetas de Windows.", L"Could not open the native folder selector of Windows."},
    {L"La ruta seleccionada no es una carpeta válida.", L"The selected path is not a valid folder."},
    {L"El selector de carpetas nativo no está disponible en esta plataforma.", L"The native folder selector is not available on this platform."},
    {L"Revisa la ruta elegida antes de preparar la importación.", L"Check the selected path before preparing the import."},
    {L"Exportación programada. Cierra MH3: después de guardar la sesión se copiará la partida al destino elegido.", L"Export scheduled. Close MH3: after saving the session, the save will be copied to the selected destination."},
    {L"Ya estás en la carpeta raíz de esta unidad.", L"You are already in the root folder of this drive."},
    {L"Carpeta actual: ", L"Current folder: "},
    {L"Importación preparada. Al cerrar MH3, se respalda la partida actual y después se sustituye.", L"Import prepared. When closing MH3, the current save is backed up and then replaced."},
    {L"La carpeta elegida no contiene data con archivos de guardado. No se ha cambiado la partida.", L"The selected folder does not contain data with save files. The save has not been changed."},
    {L"No se pudo preparar la importación. La partida actual no se modificó.", L"Could not prepare the import. The current save was not modified."},
    {L"No hay datos de partida. Abre MH3 y guarda la partida antes de pedir el backup.", L"There is no save data. Open MH3 and save the game before requesting a backup."},
    {L"Ya hay un backup programado. Se creará al cerrar MH3, después de guardar la sesión.", L"There is already a scheduled backup. It will be created when closing MH3, after the game finishes saving the session."},
    {L"Backup programado. Cierra MH3 para crearlo después de que el juego termine de guardar la sesión.", L"Backup scheduled. Close MH3 to create it after the game finishes saving the session."},
    {L"No hay una partida guardada que exportar. Abre MH3 y guarda la partida primero.", L"There is no saved game to export. Open MH3 and save the game first."},
    {L"Ya existe una importacion pendiente. Cierra MH3 para aplicarla antes de preparar otra.", L"There is already an import pending. Close MH3 to apply it before preparing another."},
    {L"Operacion cancelada. La partida actual no se ha modificado.", L"Operation canceled. The current save has not been modified."},
    {L"Importación cancelada. La partida actual no se ha modificado.", L"Import canceled. The current save has not been modified."},
    {L"Selecciona la carpeta de la partida exportada", L"Select the folder of the exported save"},
    {L"Elige dónde exportar la partida", L"Choose where to export the save"},
    {L"Elegir carpeta", L"Choose folder"},
    {L"Exportar aquí", L"Export here"},

    // ---- Overlay window title -------------------------------------------------------
    {L"Monster Hunter Tri - Ajustes", L"Monster Hunter Tri - Settings"},

    // ---- Launcher window ------------------------------------------------------------
    {L"Preparar el juego", L"Prepare the game"},
    {L"Preparando los datos necesarios. El juego se abrirá solo al terminar.", L"Preparing the necessary data. The game will open automatically when finished."},
    {L"ROM lista. Pulsa Copiar e iniciar para guardarla junto a MH3.exe.", L"ROM list. Press Copy and Start to save it next to MH3.exe."},
    {L"Hay varias ROM junto a MH3.exe. Elige la que quieres iniciar.", L"There are several ROMs next to MH3.exe. Choose the one you want to start."},
    {L"No hay ninguna ROM junto a MH3.exe. Elige tu archivo ISO, RVZ o WIA; se copiará junto al ejecutable.", L"There is no ROM next to MH3.exe. Choose your ISO, RVZ or WIA file; it will be copied next to the executable."},
    {L"Ninguna ROM seleccionada", L"No ROM selected"},
    {L"Admite ISO, RVZ y WIA", L"Supports ISO, RVZ and WIA"},
    {L"Seleccionar ROM\u2026", L"Select ROM\u2026"},
    {L"Copiar e iniciar", L"Copy and Start"},
    {L"La primera vez puede tardar un poco.", L"The first time it may take a little while."},
    {L"Esc para salir", L"Esc to exit"},
    {L"Seleccionada: ", L"Selected: "},
    {L"Iniciando preparación\u2026", L"Starting preparation\u2026"},
    {L"ROM detectada. Preparando para iniciar automáticamente.", L"ROM detected. Preparing to start automatically."},
    {L"Hay varias ROM. Selecciona una para continuar.", L"There are several ROMs. Select one to continue."},
    {L"Esperando que selecciones una ROM.", L"Waiting for you to select a ROM."},
    {L"Todos los archivos (*.*)", L"All files (*.*)"},
    {L"Copiando ROM", L"Copying ROM"},
    {L"ROM copiada; preparando archivos", L"ROM copied; preparing files"},
    {L"Listo. Abriendo el juego", L"Ready. Opening the game"},
};

std::filesystem::path UiLanguageFile() {
#ifdef _WIN32
    wchar_t path[32768]{};
    const DWORD length = GetModuleFileNameW(nullptr, path, static_cast<DWORD>(32768));
    if (length > 0 && length < 32768)
        return std::filesystem::path(std::wstring(path, length)).parent_path() / L"MH3UI.uilanguage";
#endif
    return std::filesystem::current_path() / L"MH3UI.uilanguage";
}

UiLanguage LoadUiLanguage() {
    std::ifstream in(UiLanguageFile());
    std::string code;
    if (in >> code && code == "en")
        return UiLanguage::English;
    return UiLanguage::Spanish;
}

UiLanguage& UiLanguageState() {
    static UiLanguage language = LoadUiLanguage();
    return language;
}

} // namespace

UiLanguage GetUiLanguage() {
    return UiLanguageState();
}

void SetUiLanguage(UiLanguage language) {
    UiLanguageState() = language;
    std::ofstream out(UiLanguageFile(), std::ios::trunc);
    if (out)
        out << (language == UiLanguage::English ? "en" : "es") << '\n';
}

const wchar_t* UiLanguageLabel(UiLanguage language) {
    return language == UiLanguage::English ? L"English" : L"Espa\u00f1ol";
}

const wchar_t* UiTranslate(const wchar_t* spanish) {
    if (!spanish || GetUiLanguage() == UiLanguage::Spanish)
        return spanish;
    for (const UiTranslation& entry : kTranslations) {
        if (entry.english && *entry.english && std::wcscmp(entry.spanish, spanish) == 0)
            return entry.english;
    }
    return spanish;
}
