.PHONY: clean All

All:
	@echo "----------Building project:[ flipflop_irq_vectored - Debug ]----------"
	@cd "flipflop_irq_vectored" && "$(MAKE)" -f  "flipflop_irq_vectored.mk" && "$(MAKE)" -f  "flipflop_irq_vectored.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ flipflop_irq_vectored - Debug ]----------"
	@cd "flipflop_irq_vectored" && "$(MAKE)" -f  "flipflop_irq_vectored.mk" clean
