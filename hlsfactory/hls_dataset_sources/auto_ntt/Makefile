run_hls:
	@if [ -z "$(target)" ]; then \
		echo "Error: You must provide target=..."; \
		exit 1; \
	fi
	if [ -d "$(target)_project" ]; then \
		rm -rf "$(target)_project"; \
	fi;
	mkdir $(target)_project
	cd $(target)_project; \
	cp -rf ../tcl_template/run_hls.tcl .; \
	ln -sf ../cpp/$(target).cpp .; \
	ln -sf ../cpp/ntt.h .; \
	ln -sf ../src .; \
	sed -i 's/@MODULE_NAME@/$(target)/g' run_hls.tcl; \
	vitis_hls -f run_hls.tcl

clean:
	rm -rf *_project