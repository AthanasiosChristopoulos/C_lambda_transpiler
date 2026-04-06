#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lambdalib.h"
int next_random;
int maxProducts;
int next(){
    next_random = ((int) (next_random * 1103515245 + 12345) % (int) 2147483648);
    if (next_random < 0) {
        next_random = -next_random;
    }
    
    return next_random;
}

typedef struct Category {
  StringType name;
  int id;
  void (*setCategory)(struct Category *self, StringType n, int i);
  void (*printCategory)(struct Category *self);
} Category;

void printCategory(struct Category *self) {
    write("Category: %s (ID: %d)\n", self->name, self->id);
}

void setCategory(struct Category *self, StringType n, int i) {
    self->name = n;
    self->id = i;
}

Category ctor_Category = {.printCategory = printCategory, .setCategory = setCategory};

typedef struct Product {
  StringType name;
  int id;
  double price;
  int quantity;
  Category category;
  int expiryDays;
  void (*setProduct)(struct Product *self, StringType n, int i, double p, int q, Category c, int e);
  void (*updateQuantity)(struct Product *self, int q);
  void (*updatePrice)(struct Product *self, double p);
  double (*getValue)(struct Product *self);
  void (*printProduct)(struct Product *self);
} Product;

void printProduct(struct Product *self) {
    write("Product: %s (ID: %d)\n", self->name, self->id);
    write("  Price: %.2f, Quantity: %d, Expiry: %d days\n", self->price, self->quantity, self->expiryDays);
    write("  Category: %s\n", self->category.name);
}

double getValue(struct Product *self) {
    return self->price * self->quantity;
}

void updatePrice(struct Product *self, double p) {
    self->price = p;
}

void updateQuantity(struct Product *self, int q) {
    self->quantity = self->quantity + q;
}

void setProduct(struct Product *self, StringType n, int i, double p, int q, Category c, int e) {
    self->name = n;
    self->id = i;
    self->price = p;
    self->quantity = q;
    self->category = c;
    self->expiryDays = e;
}

Product ctor_Product = {.printProduct = printProduct, .getValue = getValue, .updatePrice = updatePrice, .updateQuantity = updateQuantity, .setProduct = setProduct};


