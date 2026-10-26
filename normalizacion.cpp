#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

struct ComandaHistorica {
    char fecha[11];
    char NombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Mozo {
    int   idMozo;
    char  nombre[50];
    char  password[20];
    float totalComision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

const int MAX_MOZOS = 1000;
const int MAX_FECHAS = 1000;
const int MAX_PRODUCTOS = 2000;

void copiarCadena(char destino[], const char origen[], int tamanio) {
    int i = 0;

    while (i < tamanio - 1 && origen[i] != '\0') {
        destino[i] = origen[i];
        i++;
    }

    destino[i] = '\0';
}

void extraerApellido(char destino[], const char nombre[], int tamanio) {
    int inicio = 0;

    for (int i = 0; nombre[i] != '\0'; i++) {
        if (nombre[i] == ' ') {
            inicio = i + 1;
        }
    }

    copiarCadena(destino, nombre + inicio, tamanio);
}

void armarNombreDiario(char destino[], const char fecha[]) {
    const char prefijo[] = "comandas_";
    const char sufijo[] = ".dat";
    int pos = 0;

    for (int i = 0; prefijo[i] != '\0'; i++) {
        destino[pos++] = prefijo[i];
    }

    for (int i = 0; i < 10 && fecha[i] != '\0'; i++) {
        destino[pos++] = fecha[i];
    }

    for (int i = 0; sufijo[i] != '\0'; i++) {
        destino[pos++] = sufijo[i];
    }

    destino[pos] = '\0';
}

int buscarMozo(Mozo mozos[], int cantidad, const char nombre[]) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

int buscarFecha(char fechas[][11], int cantidad, const char fecha[]) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(fechas[i], fecha) == 0) {
            return i;
        }
    }
    return -1;
}

int buscarProducto(Producto productos[], int cantidad, int codigo) {
    for (int i = 0; i < cantidad; i++) {
        if (productos[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

bool generarMozos(Mozo mozos[], int cantidad) {
    FILE* archivo = fopen("mozos.dat", "wb");

    if (archivo == NULL) {
        cout << "Error al crear mozos.dat" << endl;
        return false;
    }

    for (int i = 0; i < cantidad; i++) {
        Mozo registro;
        memset(&registro, 0, sizeof(Mozo));

        registro.idMozo = i + 1;
        copiarCadena(registro.nombre, mozos[i].nombre, sizeof(registro.nombre));
        copiarCadena(registro.password, mozos[i].password, sizeof(registro.password));
        registro.totalComision = mozos[i].totalComision;

        if (fwrite(&registro, sizeof(Mozo), 1, archivo) != 1) {
            cout << "Error al escribir mozos.dat" << endl;
            fclose(archivo);
            return false;
        }
    }

    fclose(archivo);
    return true;
}

bool crearArchivosDiarios(char fechas[][11], int cantidadFechas) {
    for (int i = 0; i < cantidadFechas; i++) {
        char nombreArchivo[40];
        armarNombreDiario(nombreArchivo, fechas[i]);

        FILE* archivo = fopen(nombreArchivo, "wb");

        if (archivo == NULL) {
            cout << "Error al crear " << nombreArchivo << endl;
            return false;
        }

        fclose(archivo);
    }

    return true;
}

bool distribuirComandas(Mozo mozos[], int cantidadMozos) {
    FILE* historico = fopen("comandas_historicas.dat", "rb");

    if (historico == NULL) {
        cout << "Error al abrir comandas_historicas.dat" << endl;
        return false;
    }

    ComandaHistorica historica;

    while (fread(&historica, sizeof(ComandaHistorica), 1, historico) == 1) {

        int posMozo = buscarMozo(mozos, cantidadMozos, historica.NombreMozo);

        if (posMozo == -1) {
            cout << "Mozo no encontrado: " << historica.NombreMozo << endl;
            fclose(historico);
            return false;
        }

        Comanda comanda;
        memset(&comanda, 0, sizeof(Comanda));
        comanda.idMozo = mozos[posMozo].idMozo;
        comanda.codigoProducto = historica.codigoProducto;
        comanda.cantidad = historica.cantidad;
        comanda.comision = historica.comision;

        char nombreArchivo[40];
        armarNombreDiario(nombreArchivo, historica.fecha);

        FILE* diario = fopen(nombreArchivo, "ab");

        if (diario == NULL) {
            cout << "Error al abrir " << nombreArchivo << endl;
            fclose(historico);
            return false;
        }

        if (fwrite(&comanda, sizeof(Comanda), 1, diario) != 1) {
            cout << "Error al escribir " << nombreArchivo << endl;
            fclose(diario);
            fclose(historico);
            return false;
        }

        fclose(diario);
    }

    bool errorLectura = ferror(historico);
    fclose(historico);

    if (errorLectura) {
        cout << "Error al leer el historico" << endl;
        return false;
    }

    return true;
}

bool actualizarInventario() {
    Producto productos[MAX_PRODUCTOS];
    bool modificado[MAX_PRODUCTOS] = { false };
    int cantidadProductos = 0;

    FILE* inventario = fopen("inventario.dat", "rb");

    if (inventario == NULL) {
        cout << "Error al abrir inventario.dat" << endl;
        return false;
    }

    while (cantidadProductos < MAX_PRODUCTOS &&
           fread(&productos[cantidadProductos],
                 sizeof(Producto), 1, inventario) == 1) {
        cantidadProductos++;
    }

    bool errorLectura = ferror(inventario);
    fclose(inventario);

    if (errorLectura) {
        cout << "Error al leer inventario.dat" << endl;
        return false;
    }

    if (cantidadProductos == MAX_PRODUCTOS) {
        cout << "Se alcanzo el limite de productos" << endl;
        return false;
    }

    FILE* historico = fopen("comandas_historicas.dat", "rb");

    if (historico == NULL) {
        cout << "Error al abrir comandas_historicas.dat" << endl;
        return false;
    }

    ComandaHistorica comanda;

    while (fread(&comanda, sizeof(ComandaHistorica), 1, historico) == 1) {

        int pos = buscarProducto(productos, cantidadProductos, comanda.codigoProducto);

        if (pos == -1) {
            cout << "Producto no encontrado: "
                 << comanda.codigoProducto << endl;
            fclose(historico);
            return false;
        }

        productos[pos].stockActual -= comanda.cantidad;
        modificado[pos] = true;

        if (productos[pos].stockActual < 0) {
            cout << "Advertencia: stock negativo para "
                 << productos[pos].descripcion << endl;
        }
    }

    errorLectura = ferror(historico);
    fclose(historico);

    if (errorLectura) {
        cout << "Error al leer comandas_historicas.dat" << endl;
        return false;
    }

    inventario = fopen("inventario.dat", "r+b");

    if (inventario == NULL) {
        cout << "Error al abrir inventario.dat para modificarlo" << endl;
        return false;
    }

    bool escrituraCorrecta = true;

    for (int i = 0; i < cantidadProductos; i++) {
        if (!modificado[i]) {
            continue;
        }

        if (fseek(inventario, (long)i * (long)sizeof(Producto), SEEK_SET) != 0 ||
            fwrite(&productos[i], sizeof(Producto), 1, inventario) != 1) {
            cout << "Error al actualizar el producto " << productos[i].codigo << endl;
            escrituraCorrecta = false;
            break;
        }
    }

    if (fclose(inventario) != 0) {
        escrituraCorrecta = false;
    }

    return escrituraCorrecta;
}

void mostrarMozos() {
    FILE* archivo = fopen("mozos.dat", "rb");

    if (archivo == NULL) {
        cout << "Error al abrir mozos.dat" << endl;
        return;
    }

    Mozo mozo;

    cout << "          " << "MOZOS" << "          " << endl;
    cout << "Numero\tNombre\t\tPassword\tComision acumulada" << endl;

    while (fread(&mozo, sizeof(Mozo), 1, archivo) == 1) {
        cout << mozo.idMozo << "\t" << mozo.nombre << "\t" << mozo.password
             << "\t\t" << mozo.totalComision << "\n";
    }

    if (ferror(archivo)) {
        cout << "Error al leer mozos.dat" << endl;
    }

    fclose(archivo);
}

void mostrarComandas(const char nombreArchivo[]) {
    FILE* archivo = fopen(nombreArchivo, "rb");

    if (archivo == NULL) {
        cout << "Error al abrir " << nombreArchivo << endl;
        return;
    }

    Comanda comanda;

    cout << endl;
    cout << "          " << nombreArchivo << "          " << endl;
    cout << "IdMozo\tCodigo\tCantidad\tComision" << endl;

    while (fread(&comanda, sizeof(Comanda), 1, archivo) == 1) {
        cout << comanda.idMozo << "\t" << comanda.codigoProducto << "\t"
             << comanda.cantidad << "\t\t" << comanda.comision << "\n";
    }

    if (ferror(archivo)) {
        cout << "Error al leer " << nombreArchivo << endl;
    }

    fclose(archivo);
}

void mostrarInventario() {
    FILE* archivo = fopen("inventario.dat", "rb");

    if (archivo == NULL) {
        cout << "Error al abrir inventario.dat" << endl;
        return;
    }

    Producto producto;

    cout << "            " << "INVENTARIO" << "            " << endl;
    cout << "Codigo\tDescripcion\t\tPrecio\tStock" << endl;

    while (fread(&producto, sizeof(Producto), 1, archivo) == 1) {
        if (producto.codigo == 105 || producto.codigo == 112 || producto.codigo == 130) {
            cout << producto.codigo << "\t" << producto.descripcion << "\t" << producto.precio << "\t" << producto.stockActual << endl;
        }
        else {
            cout << producto.codigo << "\t" << producto.descripcion << "\t\t" << producto.precio << "\t" << producto.stockActual << endl;
        }
    }

    if (ferror(archivo)) {
        cout << "Error al leer inventario.dat" << endl;
    }

    fclose(archivo);
}

int main() {
    Mozo mozos[MAX_MOZOS];
    int cantidadMozos = 0;

    char fechas[MAX_FECHAS][11];
    int cantidadFechas = 0;

    FILE* historico = fopen("comandas_historicas.dat", "rb");

    if (historico == NULL) {
        cout << "No se pudo abrir comandas_historicas.dat" << endl;
        return 1;
    }

    ComandaHistorica comanda;

    while (fread(&comanda, sizeof(ComandaHistorica), 1, historico) == 1) {

        int posMozo = buscarMozo(mozos, cantidadMozos, comanda.NombreMozo);

        if (posMozo == -1) {
            if (cantidadMozos >= MAX_MOZOS) {
                cout << "Se alcanzo el limite de mozos" << endl;
                fclose(historico);
                return 1;
            }

            posMozo = cantidadMozos;

            memset(&mozos[posMozo], 0, sizeof(Mozo));
            copiarCadena(mozos[posMozo].nombre, comanda.NombreMozo, sizeof(mozos[posMozo].nombre));
            extraerApellido(mozos[posMozo].password, mozos[posMozo].nombre, sizeof(mozos[posMozo].password));
            mozos[posMozo].totalComision = 0;
            mozos[posMozo].idMozo = cantidadMozos + 1;

            cantidadMozos++;
        }

        mozos[posMozo].totalComision += comanda.comision;

        if (buscarFecha(fechas, cantidadFechas, comanda.fecha) == -1) {

            if (cantidadFechas >= MAX_FECHAS) {
                cout << "Se alcanzo el limite de fechas" << endl;
                fclose(historico);
                return 1;
            }

            copiarCadena(fechas[cantidadFechas], comanda.fecha, 11);
            cantidadFechas++;
        }
    }

    bool errorLectura = ferror(historico);
    fclose(historico);

    if (errorLectura) {
        cout << "Error al leer el archivo historico" << endl;
        return 1;
    }

    if (!generarMozos(mozos, cantidadMozos)) {
        return 1;
    }

    if (!crearArchivosDiarios(fechas, cantidadFechas)) {
        return 1;
    }

    if (!distribuirComandas(mozos, cantidadMozos)) {
        return 1;
    }

    if (!actualizarInventario()) {
        return 1;
    }

    mostrarMozos();

    mostrarComandas("comandas_02-06-2025.dat");

    mostrarComandas("comandas_03-06-2025.dat");

    mostrarComandas("comandas_04-06-2025.dat");

    mostrarInventario();

    return 0;
}
