log_file=testcomp.log

.BEFORE
    @%create $(log_file)

all : .SYMBOLIC
    @wmake -h -f inline.mif log_file=$(log_file) testcomp=wcc
    @wmake -h -f inline.mif log_file=$(log_file) testcomp=wcc386
    @wmake -h -f inline.mif log_file=$(log_file) testcomp=wpp
    @wmake -h -f inline.mif log_file=$(log_file) testcomp=wpp386

clean : .SYMBOLIC
    @wmake -h -f inline.mif clean
