#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

struct Mozo {
    int   idMozo;
    char  nombre[50];
    char  password[20];
    float totalComision;
};

struct Producto {
    int   codigo;
    char  descripcion[50];
    float precio;
    int   stockActual;
};

struct Comanda {
    int   idMozo;
    int   codigoProducto;
    int   cantidad;
    float comision;
};

const int   MAX_ITEMS = 10;          
const float PORC_COMISION = 0.10f;   

void agregarNumero(char* dest, int valor, int digitos) {
    if (digitos == 0) {
        int t = valor;
        digitos = 1;
        while (t >= 10) { t /= 10; digitos++; }
    }
    int largo = (int)strlen(dest);
    for (int i = digitos - 1; i >= 0; i--) {
        dest[largo + i] = (char)('0' + valor % 10);
        valor /= 10;
    }
    dest[largo + digitos] = '\0';
}

bool validarClave(const char* passwordMozo, const char* claveIngresada) {
    return strcmp(passwordMozo, claveIngresada) == 0;
}

bool buscarMozo(int id, Mozo& m) {
    FILE* f = fopen("mozos.dat", "rb");
    if (!f) { cout << "No se pudo abrir mozos.dat" << endl; return false; }
    bool ok = false;
    while (fread(&m, sizeof(Mozo), 1, f) == 1) {
        if (m.idMozo == id) { ok = true; break; }
    }
    fclose(f);
    return ok;
}

bool buscarProducto(int codigo, Producto& p) {
    FILE* f = fopen("inventario.dat", "rb");
    if (!f) { cout << "No se pudo abrir inventario.dat" << endl; return false; }
    bool ok = false;
    while (fread(&p, sizeof(Producto), 1, f) == 1) {
        if (p.codigo == codigo) { ok = true; break; }
    }
    fclose(f);
    return ok;
}

bool descontarStock(int codigo, int cantidad) {
    FILE* f = fopen("inventario.dat", "rb+");
    if (!f) { cout << "No se pudo abrir inventario.dat" << endl; return false; }
    Producto p;
    bool ok = false;
    while (fread(&p, sizeof(Producto), 1, f) == 1) {
        if (p.codigo == codigo) {
            if (p.stockActual >= cantidad) {
                p.stockActual -= cantidad;
                fseek(f, -(long)sizeof(Producto), SEEK_CUR);
                fwrite(&p, sizeof(Producto), 1, f);
                ok = true;
            }
            break;
        }
    }
    fclose(f);
    return ok;
}

int contarComandas(FILE* f) {
    fseek(f, 0, SEEK_END);
    long bytes = ftell(f);
    return (int)(bytes / (long)sizeof(Comanda));
}

void ordenarPlanilla(FILE* f) {
    int n = contarComandas(f);
    Comanda a, b;

    for (int i = 0; i < n - 1; i++) {
        bool huboCambio = false;
        for (int j = 0; j < n - 1 - i; j++) {
            long pos = (long)j * (long)sizeof(Comanda);

            fseek(f, pos, SEEK_SET);
            fread(&a, sizeof(Comanda), 1, f);
            fread(&b, sizeof(Comanda), 1, f);

            if (a.idMozo > b.idMozo) {
                fseek(f, pos, SEEK_SET);
                fwrite(&b, sizeof(Comanda), 1, f);
                fwrite(&a, sizeof(Comanda), 1, f);
                huboCambio = true;
            }
        }
        if (!huboCambio) break;   
    }
    fflush(f);
}

void mostrarPlanilla(FILE* f, const char* nombreArchivo) {
    int n = contarComandas(f);
    Comanda c;
    double totalDia = 0, comisionDia = 0;

    cout << " " << endl;
    cout << " PLANILLA DEL DIA: " << nombreArchivo << endl;
    cout << " " << endl;

    rewind(f);
    for (int i = 0; i < n; i++) {
        if (fread(&c, sizeof(Comanda), 1, f) != 1) break;

        Mozo m;
        char nombreMozo[50];
        if (buscarMozo(c.idMozo, m)) strcpy(nombreMozo, m.nombre);
        else strcpy(nombreMozo, "(desconocido)");

        double importe = c.comision / PORC_COMISION;

        cout << "Mozo " << c.idMozo << " - " << nombreMozo << " | ";
        if (c.codigoProducto == 0) {
            cout << c.cantidad << " items (detalle no disponible - planilla anterior)";
        } else {
            Producto p;
            if (!buscarProducto(c.codigoProducto, p)) strcpy(p.descripcion, "(desconocido)");
            cout << c.cantidad << " x " << p.descripcion;
        }
        cout << " = $" << importe << " | Comision: $" << c.comision << endl;

        totalDia += importe;
        comisionDia += c.comision;
    }

    cout << " " << endl;
    cout << " Lineas de venta: " << n << endl;
    cout << " Total vendido:   $" << totalDia << endl;
    cout << " Comisiones:      $" << comisionDia << endl;
    cout << endl;
}

struct ComandaVieja {
    int   idMozo;
    int   cantidad;
    float total;
    char  fecha[11];
    char  nombreMozo[50];
};   

const int MAX_VIEJAS = 2000;

bool registroViejoValido(const ComandaVieja& r) {
    if (r.idMozo <= 0 || r.idMozo > 99999) return false;
    if (r.cantidad <= 0 || r.cantidad > 1000) return false;
    if (!(r.total > 0) || r.total > 1e9f || r.total != r.total) return false;
    const char* f = r.fecha;
    if (strlen(f) != 10 || f[2] != '-' || f[5] != '-') return false;
    for (int i = 0; i < 10; i++)
        if (i != 2 && i != 5 && (f[i] < '0' || f[i] > '9')) return false;
    size_t n = 0;
    while (n < sizeof(r.nombreMozo) && r.nombreMozo[n] != '\0') n++;
    if (n == 0 || n >= sizeof(r.nombreMozo)) return false;
    for (size_t i = 0; i < n; i++)
        if ((unsigned char)r.nombreMozo[i] < 32) return false;
    return true;
}

bool esFormatoViejo(FILE* f) {
    fseek(f, 0, SEEK_END);
    long bytes = ftell(f);
    if (bytes == 0) return false;
    if (bytes % (long)sizeof(Comanda) != 0) return true;
    if (bytes >= (long)sizeof(ComandaVieja)) {
        ComandaVieja r;
        rewind(f);
        if (fread(&r, sizeof(ComandaVieja), 1, f) == 1 && registroViejoValido(r))
            return true;
    }
    return false;
}

int leerPlanillaVieja(FILE* f, ComandaVieja* out, int maxOut, int& bytesSaltados) {
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char* buf = new char[size > 0 ? size : 1];
    size_t leidos = fread(buf, 1, size, f);
    long total = (long)leidos;

    int n = 0;
    bytesSaltados = 0;
    long pos = 0;
    while (pos + (long)sizeof(ComandaVieja) <= total && n < maxOut) {
        ComandaVieja r;
        memcpy(&r, buf + pos, sizeof(ComandaVieja));
        if (registroViejoValido(r)) {
            out[n++] = r;
            pos += sizeof(ComandaVieja);
        } else {
            pos++;
            bytesSaltados++;
        }
    }
    bytesSaltados += (int)(total - pos);   
    delete[] buf;
    return n;
}

FILE* migrarPlanillaVieja(FILE* vieja, const char* nombreArchivo) {
    static ComandaVieja v[MAX_VIEJAS];
    int saltados = 0;
    int n = leerPlanillaVieja(vieja, v, MAX_VIEJAS, saltados);

    cout << "Se recuperaron " << n << " ventas de la planilla anterior." << endl;
    if (saltados > 0)
        cout << "AVISO: " << saltados << " bytes danados/incompletos no se pueden recuperar." << endl;
    cout << "Se convertira al formato actual (se pierde el detalle de productos, "
         << "se conservan mozo, cantidad y comision)." << endl;
    cout << "Migrar y continuar? (s/n): ";
    char r;
    cin >> r;
    cin.ignore(1000, '\n');
    if (r != 's' && r != 'S') return NULL;

    fclose(vieja);

    char base[60];
    strcpy(base, nombreArchivo);
    int largo = (int)strlen(base);
    if (largo > 4) base[largo - 4] = '\0';      

    char respaldo[120];
    strcpy(respaldo, base);
    strcat(respaldo, "_viejo.dat");
    for (int k = 2; ; k++) {
        FILE* t = fopen(respaldo, "rb");
        if (!t) break;
        fclose(t);
        strcpy(respaldo, base);
        strcat(respaldo, "_viejo");
        agregarNumero(respaldo, k, 0);
        strcat(respaldo, ".dat");
    }
    if (rename(nombreArchivo, respaldo) != 0) {
        cout << "No se pudo crear el respaldo." << endl;
        return NULL;
    }
    cout << "Original guardado como: " << respaldo << endl;

    FILE* nueva = fopen(nombreArchivo, "wb+");
    if (!nueva) { cout << "No se pudo crear " << nombreArchivo << endl; return NULL; }

    for (int i = 0; i < n; i++) {
        Comanda c;
        c.idMozo = v[i].idMozo;
        c.codigoProducto = 0;            
        c.cantidad = v[i].cantidad;
        c.comision = v[i].total * PORC_COMISION;
        fwrite(&c, sizeof(Comanda), 1, nueva);
    }
    fflush(nueva);
    cout << "Planilla migrada al formato actual." << endl;
    return nueva;
}

int main() {
    cout.setf(ios::fixed);
    cout.precision(2);
    int dd, mm, aaaa;
    cout << "Ingrese la fecha de hoy (dd mm aaaa)" << endl;
    cin >> dd;
    cin >> mm;
    cin >> aaaa;
    cin.ignore(1000, '\n');

    char nombreArchivo[60];
    strcpy(nombreArchivo, "comandas_");
    agregarNumero(nombreArchivo, dd, 2);
    strcat(nombreArchivo, "-");
    agregarNumero(nombreArchivo, mm, 2);
    strcat(nombreArchivo, "-");
    agregarNumero(nombreArchivo, aaaa, 4);
    strcat(nombreArchivo, ".dat");

    FILE* planilla = fopen(nombreArchivo, "rb+");
    if (!planilla) {
        planilla = fopen(nombreArchivo, "wb+");
        if (!planilla) { cout << "No se pudo crear " << nombreArchivo << endl; return 1; }
        cout << "Planilla nueva creada: " << nombreArchivo << endl;
    } else if (esFormatoViejo(planilla)) {
        cout << "Planilla en formato anterior detectada: " << nombreArchivo << endl;
        planilla = migrarPlanillaVieja(planilla, nombreArchivo);
        if (!planilla) return 0;
    } else {
        cout << "Planilla existente abierta: " << nombreArchivo << endl;
    }

    char seguir = 's';

    while (seguir == 's' || seguir == 'S') {
        cout << "     " << "Nueva venta" << "     " << endl;

        Mozo mozo;
        int idMozo;
        cout << "Numero de mozo: ";
        cin >> idMozo;
        cin.ignore(1000, '\n');

        if (!buscarMozo(idMozo, mozo)) {
            cout << "Mozo inexistente." << endl;
        } else {
            bool autenticado = false;
            for (int intento = 1; intento <= 3 && !autenticado; intento++) {
                char clave[50];
                cout << "Clave: ";
                cin.getline(clave, 50);
                if (validarClave(mozo.password, clave)) autenticado = true;
                else cout << "Clave incorrecta (" << intento << "/3)" << endl;
            }

            if (autenticado) {
                cout << "Bienvenido/a " << mozo.nombre << endl;

                int items = 0;
                double totalVenta = 0, comisionVenta = 0;

                cout << "Cargue los productos (codigo 0 para terminar)" << endl;
                while (items < MAX_ITEMS) {
                    int cod, cant;
                    cout << "Codigo de producto: ";
                    cin >> cod;
                    if (cod == 0) break;

                    Producto p;
                    if (!buscarProducto(cod, p)) {
                        cout << "Producto inexistente." << endl;
                        continue;
                    }
                    
                    cout << "  " << p.descripcion << " - $" << p.precio << endl;
                    cout << "Cantidad: ";
                    cin >> cant;
                    if (cant <= 0) { cout << "Cantidad invalida." << endl; continue; }

                    if (cant > p.stockActual) {
                        cout << "No hay suficientes existencias (disponible: "
                             << p.stockActual << ")" << endl;
                        continue;
                    }

                    if (!descontarStock(p.codigo, cant)) {
                        cout << "No se pudo descontar el stock." << endl;
                        continue;
                    }

                    Comanda c;
                    c.idMozo = mozo.idMozo;
                    c.codigoProducto = p.codigo;
                    c.cantidad = cant;
                    c.comision = p.precio * cant * PORC_COMISION;
                    fseek(planilla, 0, SEEK_END);
                    fwrite(&c, sizeof(Comanda), 1, planilla);
                    fflush(planilla);

                    totalVenta += p.precio * cant;
                    comisionVenta += c.comision;
                    items++;
                }
                cin.ignore(1000, '\n');

                if (items > 0) {
                    cout << "Total de la venta: $" << totalVenta << endl;
                    cout << "Comision (10%): $" << comisionVenta << endl;
                    cout << "Venta registrada." << endl;
                } else {
                    cout << "Venta sin productos, no se registra." << endl;
                }
            } else {
                cout << "Acceso denegado." << endl;
            }
        }

        cout << "Otra venta? (s/n): ";
        cin >> seguir;
        cin.ignore(1000, '\n');
    }

    ordenarPlanilla(planilla);
    cout << "Planilla " << nombreArchivo << " guardada y ordenada por mozo." << endl;

    mostrarPlanilla(planilla, nombreArchivo);
    fclose(planilla);
    return 0;
}
