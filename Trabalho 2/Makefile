CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -pthread -g
SRCDIR = src
OBJDIR = obj

TARGET_SERVER = servidor_rmi
TARGET_CLIENT = cliente_rmi
TARGET_TESTES = cliente_testes  # ADICIONAR ESTA LINHA

all: $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_TESTES)  # ADICIONAR $(TARGET_TESTES)

$(TARGET_SERVER): $(OBJDIR)/servidor_rmi.o $(OBJDIR)/rmi_core.o $(OBJDIR)/serializer.o
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(TARGET_CLIENT): $(OBJDIR)/cliente_rmi.o $(OBJDIR)/rmi_core.o $(OBJDIR)/serializer.o
	$(CXX) $^ -o $@ $(CXXFLAGS)

# ADICIONAR ESTAS 2 LINhas:
$(TARGET_TESTES): $(OBJDIR)/cliente_testes.o $(OBJDIR)/rmi_core.o $(OBJDIR)/serializer.o
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_TESTES)  # ADICIONAR $(TARGET_TESTES)

.PHONY: all clean