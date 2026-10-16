#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

struct comandas_historicas{
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct producto{
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct mozo{
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct comanda{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const int MAX_PRODUCTOS = 100;
const int MAX_HIST      = 500;
const int MAX_MOZOS     = 50;
const int MAX_DIAS      = 62;
const int DESPLAZAMIENTO = 7;   

void encriptar(const char *origen, char *destino, int tam){
    int i = 0;
    while(origen[i] != '\0' && i < tam - 1){
        destino[i] = (char)((unsigned char)origen[i] + DESPLAZAMIENTO);
        i++;
    }
    destino[i] = '\0';
}

void desencriptar(const char *origen, char *destino, int tam){
    int i = 0;
    while(origen[i] != '\0' && i < tam - 1){
        destino[i] = (char)((unsigned char)origen[i] - DESPLAZAMIENTO);
        i++;
    }
    destino[i] = '\0';
}

bool verificarPassword(const mozo &m, const char *intento){
    char tmp[20];
    encriptar(intento, tmp, 20);
    return strcmp(tmp, m.password) == 0;
}

void ordenarPorMozo(comanda v[], int n){
    for(int i = 1; i < n; i++){
        comanda aux = v[i];
        int j = i - 1;
        while(j >= 0 && v[j].idMozo > aux.idMozo){
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = aux;
    }
}

int buscarMozo(mozo mozos[], int cant, const char *nombre){
    for(int i = 0; i < cant; i++)
        if(strcmp(mozos[i].nombre, nombre) == 0) return i;
    return -1;
}

int buscarProducto(producto prods[], int cant, int codigo){
    for(int i = 0; i < cant; i++)
        if(prods[i].codigo == codigo) return i;
    return -1;
}

void imprimirPassword(const char *pass){
    for(int i = 0; pass[i] != '\0'; i++)
        printf("%d ", (unsigned char)pass[i]);
}

int main(){
    producto productos[MAX_PRODUCTOS];
    comandas_historicas hist[MAX_HIST];
    mozo mozos[MAX_MOZOS];
    int idDeHist[MAX_HIST];
    char fechas[MAX_DIAS][11];
    int cantProd = 0, cantHist = 0, cantMozos = 0, cantDias = 0;

    FILE *f = fopen("inventario.dat", "rb");
    if(!f){ cout << "No se pudo abrir inventario.dat" << endl; return 1; }
    cantProd = (int)fread(productos, sizeof(producto), MAX_PRODUCTOS, f);
    fclose(f);

    f = fopen("comandas_historicas.dat", "rb");
    if(!f){ cout << "No se pudo abrir comandas_historicas.dat" << endl; return 1; }
    cantHist = (int)fread(hist, sizeof(comandas_historicas), MAX_HIST, f);
    fclose(f);

    for(int i = 0; i < cantHist; i++){
        int pos = buscarMozo(mozos, cantMozos, hist[i].nombreMozo);
        if(pos == -1){
            if(cantMozos >= MAX_MOZOS){ cout << "Demasiados mozos" << endl; return 1; }
            pos = cantMozos++;
            mozos[pos].idMozo = pos + 1;
            strcpy(mozos[pos].nombre, hist[i].nombreMozo);
            encriptar(hist[i].nombreMozo, mozos[pos].password, 20);
            mozos[pos].totalComision = 0;
        }
        mozos[pos].totalComision += hist[i].comision;
        idDeHist[i] = mozos[pos].idMozo;
    }

    f = fopen("mozos.dat", "wb");
    if(!f){ cout << "No se pudo crear mozos.dat" << endl; return 1; }
    for(int i = 0; i < cantMozos; i++){
        fwrite(&mozos[i].idMozo, sizeof(int), 1, f);
        fwrite(mozos[i].nombre, sizeof(char), 50, f);
        fwrite(mozos[i].password, sizeof(char), 20, f);
    }
    fclose(f);

    for(int i = 0; i < cantHist; i++){
        bool existe = false;
        for(int d = 0; d < cantDias; d++)
            if(strcmp(fechas[d], hist[i].fecha) == 0){ existe = true; break; }
        if(!existe && cantDias < MAX_DIAS)
            strcpy(fechas[cantDias++], hist[i].fecha);
    }

    for(int d = 0; d < cantDias; d++){
        comanda dia[MAX_HIST];
        int n = 0;
        for(int i = 0; i < cantHist; i++){
            if(strcmp(hist[i].fecha, fechas[d]) == 0){
                dia[n].idMozo         = idDeHist[i];
                dia[n].codigoProducto = hist[i].codigoProducto;
                dia[n].cantidad       = hist[i].cantidad;
                dia[n].comision       = hist[i].comision;
                n++;
            }
        }
        ordenarPorMozo(dia, n);

        char nombreArchivo[40];
        sprintf(nombreArchivo, "comandas_%s.dat", fechas[d]);
        f = fopen(nombreArchivo, "wb");
        if(!f){ cout << "No se pudo crear " << nombreArchivo << endl; return 1; }
        size_t escritos = fwrite(dia, sizeof(comanda), n, f);
        fflush(f);
        if(fclose(f) != 0 || (int)escritos != n)
            cout << "ERROR al escribir " << nombreArchivo << " (" << escritos << " de " << n << " registros)" << endl;
        else
            cout << "Guardado " << nombreArchivo << ": " << n << " comandas" << endl;
    }

    for(int i = 0; i < cantHist; i++){
        int p = buscarProducto(productos, cantProd, hist[i].codigoProducto);
        if(p == -1){
            cout << "Aviso: producto " << hist[i].codigoProducto << " inexistente en inventario" << endl;
        } else if(productos[p].stockActual < hist[i].cantidad){
            cout << "Aviso: stock insuficiente del producto " << productos[p].codigo
                 << " (" << productos[p].descripcion << "), queda en 0" << endl;
            productos[p].stockActual = 0;
        } else {
            productos[p].stockActual -= hist[i].cantidad;
        }
    }
    f = fopen("inventario.dat", "wb");
    if(!f){ cout << "No se pudo escribir inventario.dat" << endl; return 1; }
    fwrite(productos, sizeof(producto), cantProd, f);
    fclose(f);

    cout << "     " << " MOZOS (mozos.dat) " << " " << endl;
    f = fopen("mozos.dat", "rb");
    if(f){
        mozo m;
        while(fread(&m.idMozo, sizeof(int), 1, f) == 1 &&
              fread(m.nombre, sizeof(char), 50, f) == 50 &&
              fread(m.password, sizeof(char), 20, f) == 20){
            char claro[20];
            desencriptar(m.password, claro, 20);
            printf("Mozo %d: %s", m.idMozo, m.nombre);
            printf("   Password encriptado (codigos ASCII+7): ");
            imprimirPassword(m.password);
            printf("\n   Password desencriptado (texto): %s", claro);
            printf("   Comision total*: $%.2f", mozos[m.idMozo - 1].totalComision);
        }
        fclose(f);
        cout << "* La comision NO se guarda en mozos.dat; se calcula al procesar." << endl;
    }

    cout << "     " << "VERIFICACION DE PASSWORDS" << " " << endl;
    for(int i = 0; i < cantMozos; i++){
        printf("%-20s password=nombre: %-9s | password 'incorrecta': %s\n",
               mozos[i].nombre,
               verificarPassword(mozos[i], mozos[i].nombre) ? "ACEPTADO" : "RECHAZADO",
               verificarPassword(mozos[i], "incorrecta") ? "ACEPTADO" : "RECHAZADO");
    }

    for(int d = 0; d < cantDias; d++){
        char nombreArchivo[40];
        sprintf(nombreArchivo, "comandas_%s.dat", fechas[d]);
        cout << "\n===== " << nombreArchivo << " =====" << endl;
        f = fopen(nombreArchivo, "rb");
        if(!f) continue;
        comanda c;
        printf("%-7s %-10s %-9s %s\n", "Mozo", "Producto", "Cantidad", "Comision");
        while(fread(&c, sizeof(comanda), 1, f) == 1)
            printf("%-7d %-10d %-9d $%.2f\n", c.idMozo, c.codigoProducto, c.cantidad, c.comision);
        fclose(f);
    }

    cout << "\n===== INVENTARIO ACTUALIZADO (inventario.dat) =====" << endl;
    f = fopen("inventario.dat", "rb");
    if(f){
        producto p;
        printf("%-8s %-20s %-10s %s\n", "Codigo", "Descripcion", "Precio", "Stock");
        while(fread(&p, sizeof(producto), 1, f) == 1)
            printf("%-8d %-20s $%-9.2f %d\n", p.codigo, p.descripcion, p.precio, p.stockActual);
        fclose(f);
    }

    return 0;
}
