#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    float price;
    int stock;
    float discount;
} Product;

typedef struct CartItem {
    Product product;
    int quantity;
    struct CartItem* next;
} CartItem;

int initialProductCapacity = 20;
Product* products;
int numProducts = 0;
CartItem* cart = NULL;

void displayProducts();
int binarySearchByName(char* productName);
void addToCart(char* productName, int quantity);
void viewCart();
void removeFromCart(char* productName);
void checkout();
void adminAddProduct(int id, char* name, float price, int stock, float discount);
void adminRestockProduct(char* productName, int stock);
void adminRemoveProduct(char* productName);
void adminSetDiscount(char* productName, float discount);
float applyPromoCode(char* promoCode);
void loadProductsFromFile();
void saveProductsToFile();

int main() {
    int choice;
    char productName[50];
    int quantity, stock;
    const char adminPassword[] = "tamim";
    char enteredPassword[50];

    products = (Product*)malloc(initialProductCapacity * sizeof(Product));
    if (products == NULL) {
        printf("Failed to allocate memory for products array.\n");
        exit(1);
    }

    loadProductsFromFile();

    while (1) {
        printf("\n========= E-Commerce Menu =========\n");
        printf("1. View Products\n");
        printf("2. Add to Cart\n");
        printf("3. View Cart\n");
        printf("4. Remove from Cart\n");
        printf("5. Checkout\n");
        printf("6. Admin: Add Product\n");
        printf("7. Admin: Restock Product\n");
        printf("8. Admin: Remove Product\n");
        printf("9. Admin: Set Discount\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayProducts();
                break;
            case 2:
                printf("Enter Product Name to add: ");
                scanf("%s", productName);
                printf("Enter Quantity: ");
                scanf("%d", &quantity);
                addToCart(productName, quantity);
                break;
            case 3:
                viewCart();
                break;
            case 4:
                printf("Enter Product Name to remove: ");
                scanf("%s", productName);
                removeFromCart(productName);
                break;
            case 5:
                checkout();
                break;
            case 6:
            case 7:
            case 8:
            case 9:
                printf("Enter Admin Password: ");
                scanf("%s", enteredPassword);
                if (strcmp(enteredPassword, adminPassword) == 0) {
                    if (choice == 6) {
                        char name[50];
                        float price, discount;
                        printf("Enter Product Name: ");
                        scanf("%s", name);
                        printf("Enter Product Price: ");
                        scanf("%f", &price);
                        printf("Enter Product Stock: ");
                        scanf("%d", &stock);
                        printf("Enter Discount Percentage: ");
                        scanf("%f", &discount);
                        adminAddProduct(numProducts + 1, name, price, stock, discount);
                    } else if (choice == 7) {
                        printf("Enter Product Name to restock: ");
                        scanf("%s", productName);
                        printf("Enter Stock Quantity: ");
                        scanf("%d", &stock);
                        adminRestockProduct(productName, stock);
                    } else if (choice == 8) {
                        printf("Enter Product Name to remove: ");
                        scanf("%s", productName);
                        adminRemoveProduct(productName);
                    } else if (choice == 9) {
                        printf("Enter Product Name to set discount: ");
                        scanf("%s", productName);
                        float discount;
                        printf("Enter Discount Percentage: ");
                        scanf("%f", &discount);
                        adminSetDiscount(productName, discount);
                    }
                } else {
                    printf("Incorrect password! Access denied.\n");
                }
                break;
            case 10:
                saveProductsToFile();
                free(products);
                exit(0);
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

void displayProducts() {
    printf("\n==================== Available Products ====================\n");
    printf(" ID   %-20s %-10s %-6s %-6s\n", "Name", "Price", "Stock", "Discount");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < numProducts; i++) {
        printf(" %-4d %-20s %-10.2f %-6d %-6.2f%%\n", 
               products[i].id, products[i].name, products[i].price, products[i].stock, products[i].discount);
    }
    printf("===========================================================\n");
}

int binarySearchByName(char* productName) {
    int low = 0, high = numProducts - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(products[mid].name, productName);
        if (cmp == 0) return mid;
        else if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void addToCart(char* productName, int quantity) {
    int index = binarySearchByName(productName);
    if (index == -1) {
        printf("Product not found!\n");
        return;
    }
    if (products[index].stock < quantity) {
        printf("Insufficient stock available! Available: %d\n", products[index].stock);
        return;
    }
    CartItem* newItem = (CartItem*)malloc(sizeof(CartItem));
    newItem->product = products[index];
    newItem->quantity = quantity;
    newItem->next = NULL;
    products[index].stock -= quantity;
    if (cart == NULL) {
        cart = newItem;
    } else {
        CartItem* temp = cart;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newItem;
    }
    printf("%s has been added to the cart.\n", products[index].name);
}

void viewCart() {
    if (cart == NULL) {
        printf("Your cart is empty.\n");
        return;
    }
    printf("\n===================== Your Cart =====================\n");
    printf(" ID   %-20s %-10s %-10s %-10s %-10s\n", "Name", "Price", "Discount", "Quantity", "Total");
    printf("----------------------------------------------------\n");
    CartItem* temp = cart;
    float totalAmount = 0;
    while (temp != NULL) {
        float discountedPrice = temp->product.price * (1 - temp->product.discount / 100);
        float totalPrice = discountedPrice * temp->quantity;
        printf(" %-4d %-20s %-10.2f %-10.2f%% %-10d %-10.2f\n", 
       temp->product.id, temp->product.name, temp->product.price, 
       temp->product.discount, temp->quantity, totalPrice);
        totalAmount += totalPrice;
        temp = temp->next;
    }
    printf("----------------------------------------------------\n");
    printf(" %-4s %-20s %-10s %-10s %-10.2f\n", "", "", "", "Total:", totalAmount);
    printf("====================================================\n");
}

void removeFromCart(char* productName) {
    if (cart == NULL) {
        printf("Your cart is empty.\n");
        return;
    }
    CartItem* temp = cart;
    CartItem* prev = NULL;
    while (temp != NULL && strcmp(temp->product.name, productName) != 0) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("Product not found in cart!\n");
        return;
    }
    products[binarySearchByName(temp->product.name)].stock += temp->quantity;
    if (prev == NULL) {
        cart = temp->next;
    } else {
        prev->next = temp->next;
    }
    free(temp);
    printf("Product removed from the cart.\n");
}

void checkout() {
    if (cart == NULL) {
        printf("Your cart is empty.\n");
        return;
    }
    viewCart();

    printf("Enter Promo Code (or 'NONE' if you don't have one): ");
    char promoCode[50];
    scanf("%s", promoCode);
    float promoDiscountPercent = applyPromoCode(promoCode);

    float totalAmount = 0;
    CartItem* temp = cart;
    while (temp != NULL) {
        float discountedPrice = temp->product.price * (1 - temp->product.discount / 100);
        totalAmount += discountedPrice * temp->quantity;
        temp = temp->next;
    }

    float discountAmount = totalAmount * (promoDiscountPercent / 100);
    totalAmount -= discountAmount;

    printf("\n===================== Final Bill =====================\n");
    printf("Subtotal: %.2f\n", totalAmount + discountAmount);
    printf("Promo Code Discount: %.2f%% (%.2f)\n", promoDiscountPercent, discountAmount);
    printf("Total Payable: %.2f\n", totalAmount);
    printf("=====================================================\n");

    printf("Checkout complete! Thank you for your purchase.\n");

    while (cart != NULL) {
        CartItem* temp = cart;
        cart = cart->next;
        free(temp);
    }
}

void adminAddProduct(int id, char* name, float price, int stock, float discount) {
    if (numProducts >= initialProductCapacity) {
        initialProductCapacity *= 2;
        products = (Product*)realloc(products, initialProductCapacity * sizeof(Product));
        if (products == NULL) {
            printf("Failed to reallocate memory for products array.\n");
            exit(1);
        }
    }
    products[numProducts].id = id;
    strcpy(products[numProducts].name, name);
    products[numProducts].price = price;
    products[numProducts].stock = stock;
    products[numProducts].discount = discount;
    numProducts++;
    printf("Product added successfully with ID %d!\n", id);
}

void adminRestockProduct(char* productName, int stock) {
    int index = binarySearchByName(productName);
    if (index == -1) {
        printf("Product not found!\n");
        return;
    }
    products[index].stock += stock;
    printf("Product restocked successfully! New stock: %d\n", products[index].stock);
}

void adminRemoveProduct(char* productName) {
    int index = binarySearchByName(productName);
    if (index == -1) {
        printf("Product not found!\n");
        return;
    }
    for (int i = index; i < numProducts - 1; i++) {
        products[i] = products[i + 1];
    }
    numProducts--;
    printf("Product removed successfully.\n");
}

void adminSetDiscount(char* productName, float discount) {
    int index = binarySearchByName(productName);
    if (index == -1) {
        printf("Product not found!\n");
        return;
    }
    products[index].discount = discount;
    printf("Discount set to %.2f%% for product %s.\n", discount, products[index].name);
}

float applyPromoCode(char* promoCode) {
    if (strcmp(promoCode, "BlackFriday") == 0) {
        return 10.0;
    }
    if (strcmp(promoCode, "NewYear") == 0) {
        return 50.0;
    }
    if (strcmp(promoCode, "NONE") == 0) {
        return 0.0; 
    }
    printf("Invalid promo code entered! No discount applied.\n");
    return 0.0; 
}

void loadProductsFromFile() {
    FILE *file = fopen("products.txt", "r");
    if (!file) {
        printf("No existing product file found, starting with initial products.\n");
        return;
    }
    while (fscanf(file, "%d %49s %f %d %f", &products[numProducts].id, products[numProducts].name,
                  &products[numProducts].price, &products[numProducts].stock, &products[numProducts].discount) == 5) {
        numProducts++;
    }
    fclose(file);
}

void saveProductsToFile() {
    FILE *file = fopen("products.txt", "w");
    if (!file) {
        printf("Error opening file for saving products.\n");
        return;
    }
    for (int i = 0; i < numProducts; i++) {
        fprintf(file, "%d %s %.2f %d %.2f\n", products[i].id, products[i].name, products[i].price, products[i].stock, products[i].discount);
    }
    fclose(file);
}
